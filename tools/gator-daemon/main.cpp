/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include "CCNDriver.h"
#include "Child.h"
#include "EventsXML.h"
#include "Logging.h"
#include "Monitor.h"
#include "OlySocket.h"
#include "OlyUtility.h"
#include "SessionData.h"
#include "Setup.h"

extern Child* child;
static int shutdownFilesystem();
static pthread_mutex_t numSessions_mutex;
static OlyServerSocket* sock = NULL;
static Monitor monitor;
static int numSessions = 0;
static bool driverRunningAtStart = false;
static bool driverMountedAtStart = false;

struct cmdline_t {
	char *module;
	int port;
	bool update;
};

#define DEFAULT_PORT 8080

void cleanUp() {
	if (shutdownFilesystem() == -1) {
		logg->logMessage("Error shutting down gator filesystem");
	}
	delete sock;
	delete util;
	delete logg;
}

//                      
static void handler(int signum) {
	logg->logMessage("Received signal %d, gator daemon exiting", signum);

	//                                                 
	if (numSessions > 0) {
		//                                                                
		//                                                                   
		sleep(1);
	}

	//                                            
	if (numSessions > 0) {
		//                                                       
		logg->logMessage("Killing process group as %d child was running when signal was received", numSessions);
		kill(0, SIGINT);

		//                                
		sleep(1);

		if (numSessions > 0) {
			//                                        
			logg->logMessage("Force kill the child");
			kill(0, SIGINT);
			//                          
			sleep(1);

			if (numSessions > 0) {
				//                                                                                                                
				printf("Unable to kill the gatord child process, thus gator.ko may still be loaded.\n");
			}
		}
	}

	cleanUp();
	exit(0);
}

//                          
static void child_exit(int) {
	int status;
	int pid = wait(&status);
	if (pid != -1) {
		pthread_mutex_lock(&numSessions_mutex);
		numSessions--;
		pthread_mutex_unlock(&numSessions_mutex);
		logg->logMessage("Child process %d exited with status %d", pid, status);
	}
}

static const int UDP_REQ_PORT = 30001;

typedef struct {
	char rviHeader[8];
	uint32_t messageID;
	uint8_t ethernetAddress[8];
	uint32_t ethernetType;
	uint32_t dhcp;
	char dhcpName[40];
	uint32_t ipAddress;
	uint32_t defaultGateway;
	uint32_t subnetMask;
	uint32_t activeConnections;
} RVIConfigureInfo;

static const char DST_REQ[] = { 'D', 'S', 'T', '_', 'R', 'E', 'Q', ' ', 0, 0, 0, 0x64 };

class UdpListener {
public:
	UdpListener() : mDstAns(), mReq(-1) {}

	void setup(int port) {
		mReq = udpPort(UDP_REQ_PORT);

		//                         
		memset(&mDstAns, 0, sizeof(mDstAns));
		memcpy(mDstAns.rviHeader, "STR_ANS ", sizeof(mDstAns.rviHeader));
		if (gethostname(mDstAns.dhcpName, sizeof(mDstAns.dhcpName) - 1) != 0) {
			logg->logError(__FILE__, __LINE__, "gethostname failed");
			handleException();
		}
		//                                                     
		if (port != DEFAULT_PORT) {
			mDstAns.defaultGateway = port;
		}
		//                                                      
		mDstAns.subnetMask = PROTOCOL_VERSION;
	}

	int getReq() const {
		return mReq;
	}

	void handle() {
		char buf[128];
		struct sockaddr_in6 sockaddr;
		socklen_t addrlen;
		int read;
		addrlen = sizeof(sockaddr);
		read = recvfrom(mReq, &buf, sizeof(buf), 0, (struct sockaddr *)&sockaddr, &addrlen);
		if (read < 0) {
			logg->logError(__FILE__, __LINE__, "recvfrom failed");
			handleException();
		} else if ((read == 12) && (memcmp(buf, DST_REQ, sizeof(DST_REQ)) == 0)) {
			//                                                                                           
			sendto(mReq, &mDstAns, sizeof(mDstAns), 0, (struct sockaddr *)&sockaddr, addrlen);
		}
	}

	void close() {
		::close(mReq);
	}

private:
	int udpPort(int port) {
		int s;
		struct sockaddr_in6 sockaddr;
		int on;
		int family = AF_INET6;

		s = socket_cloexec(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		if (s == -1) {
			family = AF_INET;
			s = socket_cloexec(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (s == -1) {
				logg->logError(__FILE__, __LINE__, "socket failed");
				handleException();
			}
		}

		on = 1;
		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)) != 0) {
			logg->logError(__FILE__, __LINE__, "setsockopt failed");
			handleException();
		}

		memset((void*)&sockaddr, 0, sizeof(sockaddr));
		sockaddr.sin6_family = family;
		sockaddr.sin6_port = htons(port);
		sockaddr.sin6_addr = in6addr_any;
		if (bind(s, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
			logg->logError(__FILE__, __LINE__, "socket failed");
			handleException();
		}

		return s;
	}

	RVIConfigureInfo mDstAns;
	int mReq;
};

static UdpListener udpListener;

//                                                                        
static int mountGatorFS() {
	//                    
	if (access("/dev/gator/buffer", F_OK) == 0) {
		return 0;
	}

	//                           
	mkdir("/dev/gator", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (mount("nodev", "/dev/gator", "gatorfs", 0, NULL) != 0) {
		return -1;
	} else {
		return 1;
	}
}

static bool init_module (const char * const location) {
	bool ret(false);
	const int fd = open(location, O_RDONLY | O_CLOEXEC);
	if (fd >= 0) {
		struct stat st;
		if (fstat(fd, &st) == 0) {
			void * const p = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
			if (p != MAP_FAILED) {
				if (syscall(__NR_init_module, p, (unsigned long)st.st_size, "") == 0) {
					ret = true;
				}
				munmap(p, st.st_size);
			}
		}
		close(fd);
	}

	return ret;
}

static bool setupFilesystem(char* module) {
	if (module) {
		//                                                                                                                          
		shutdownFilesystem();

		//                 
		if (access("/dev/gator/buffer", F_OK) == 0) {
			logg->logError(__FILE__, __LINE__, "Unable to remove the running gator.ko. Manually remove the module or use the running module by not specifying one on the commandline");
			handleException();
		}
	}

	const int retval = mountGatorFS();
	if (retval == 1) {
		logg->logMessage("Driver already running at startup");
		driverRunningAtStart = true;
	} else if (retval == 0) {
		logg->logMessage("Driver already mounted at startup");
		driverRunningAtStart = driverMountedAtStart = true;
	} else {
		char command[256]; //                         
		char location[256]; //                         

		if (module) {
			strncpy(location, module, sizeof(location));
		} else {
			//                                                
			if (util->getApplicationFullPath(location, sizeof(location)) != 0) { //                        
				logg->logMessage("Unable to determine the full path of gatord, the cwd will be used");
			}
			strncat(location, "gator.ko", sizeof(location) - strlen(location) - 1);
		}

		if (access(location, F_OK) == -1) {
			if (module == NULL) {
				//                                                                                             
				return false;
			} else {
				//                                                                  
				logg->logError(__FILE__, __LINE__, "gator module not found at %s", location);
				handleException();
			}
		}

		//            
		bool success = init_module(location);
		if (!success) {
			logg->logMessage("init_module failed, trying insmod");
			snprintf(command, sizeof(command), "insmod %s >/dev/null 2>&1", location);
			if (system(command) != 0) {
				logg->logMessage("Unable to load gator.ko driver with command: %s", command);
				logg->logError(__FILE__, __LINE__, "Unable to load (insmod) gator.ko driver:\n  >>> gator.ko must be built against the current kernel version & configuration\n  >>> See dmesg for more details");
				handleException();
			}
		}

		if (mountGatorFS() == -1) {
			logg->logError(__FILE__, __LINE__, "Unable to mount the gator filesystem needed for profiling.");
			handleException();
		}
	}

	return true;
}

static int shutdownFilesystem() {
	if (driverMountedAtStart == false) {
		umount("/dev/gator");
	}
	if (driverRunningAtStart == false) {
		if (syscall(__NR_delete_module, "gator", O_NONBLOCK) != 0) {
			logg->logMessage("delete_module failed, trying rmmod");
			if (system("rmmod gator >/dev/null 2>&1") != 0) {
				return -1;
			}
		}
	}

	return 0; //        
}

static const char OPTSTRING[] = "hvudap:s:c:e:m:o:";

static bool hasDebugFlag(int argc, char** argv) {
	int c;

	optind = 1;
	while ((c = getopt(argc, argv, OPTSTRING)) != -1) {
		if (c == 'd') {
			return true;
		}
	}

	return false;
}

static struct cmdline_t parseCommandLine(int argc, char** argv) {
	struct cmdline_t cmdline;
	cmdline.port = DEFAULT_PORT;
	cmdline.module = NULL;
	cmdline.update = false;
	char version_string[256]; //                                                 
	int c;

	//                         
	if (PROTOCOL_VERSION < PROTOCOL_DEV) {
		snprintf(version_string, sizeof(version_string), "Streamline gatord version %d (DS-5 v5.%d)", PROTOCOL_VERSION, PROTOCOL_VERSION);
	} else {
		snprintf(version_string, sizeof(version_string), "Streamline gatord development version %d", PROTOCOL_VERSION);
	}

	optind = 1;
	while ((c = getopt(argc, argv, OPTSTRING)) != -1) {
		switch (c) {
			case 'c':
				gSessionData->mConfigurationXMLPath = optarg;
				break;
			case 'd':
				//                
				break;
			case 'e':
				gSessionData->mEventsXMLPath = optarg;
				break;
			case 'm':
				cmdline.module = optarg;
				break;
			case 'p':
				cmdline.port = strtol(optarg, NULL, 10);
				break;
			case 's':
				gSessionData->mSessionXMLPath = optarg;
				break;
			case 'o':
				gSessionData->mTargetPath = optarg;
				break;
			case 'u':
				cmdline.update = true;
				break;
			case 'a':
				gSessionData->mAllowCommands = true;
				break;
			case 'h':
			case '?':
				logg->logError(__FILE__, __LINE__,
					"%s. All parameters are optional:\n"
					"-c config_xml   path and filename of the configuration.xml to use\n"
					"-e events_xml   path and filename of the events.xml to use\n"
					"-h              this help page\n"
					"-m module       path and filename of gator.ko\n"
					"-p port_number  port upon which the server listens; default is 8080\n"
					"-s session_xml  path and filename of a session.xml used for local capture\n"
					"-o apc_dir      path and name of the output for a local capture\n"
					"-v              version information\n"
					"-d              enable debug messages\n"
					"-a              allow the user user to provide a command to run at the start of a capture"
					, version_string);
				handleException();
				break;
			case 'v':
				logg->logError(__FILE__, __LINE__, version_string);
				handleException();
				break;
		}
	}

	//               
	if (cmdline.port != DEFAULT_PORT && gSessionData->mSessionXMLPath != NULL) {
		logg->logError(__FILE__, __LINE__, "Only a port or a session xml can be specified, not both");
		handleException();
	}

	if (gSessionData->mTargetPath != NULL && gSessionData->mSessionXMLPath == NULL) {
		logg->logError(__FILE__, __LINE__, "Missing -s command line option required for a local capture.");
		handleException();
	}

	if (optind < argc) {
		logg->logError(__FILE__, __LINE__, "Unknown argument: %s. Use '-h' for help.", argv[optind]);
		handleException();
	}

	return cmdline;
}

static void handleClient() {
	OlySocket client(sock->acceptConnection());

	int pid = fork();
	if (pid < 0) {
		//      
		logg->logError(__FILE__, __LINE__, "Fork process failed. Please power cycle the target device if this error persists.");
	} else if (pid == 0) {
		//      
		sock->closeServerSocket();
		udpListener.close();
		monitor.close();
		child = new Child(&client, numSessions + 1);
		child->run();
		delete child;
		exit(0);
	} else {
		//       
		client.closeSocket();

		pthread_mutex_lock(&numSessions_mutex);
		numSessions++;
		pthread_mutex_unlock(&numSessions_mutex);

		//                                   
		int wait = 0;
		while (numSessions > 1) {
			//                                                                                               
			logg->logMessage("%d sessions active!", numSessions);
			if (wait++ >= 10) { //                             
				//                        
				kill(pid, SIGALRM);
				break;
			}
			sleep(1);
		}
	}
}

//                                                       
int main(int argc, char** argv) {
	//                                                                        
	//                                                                     
	setsid();

  //                                  
	logg = new Logging(hasDebugFlag(argc, argv));
	//                  
	gSessionData = new SessionData();
	//                            
	util = new OlyUtility();

	//                   
	new CCNDriver();

	prctl(PR_SET_NAME, (unsigned long)&"gatord-main", 0, 0, 0);
	pthread_mutex_init(&numSessions_mutex, NULL);

	signal(SIGINT, handler);
	signal(SIGTERM, handler);
	signal(SIGABRT, handler);

	//                     
	if (setpriority(PRIO_PROCESS, syscall(__NR_gettid), -19) == -1) {
		logg->logMessage("setpriority() failed");
	}

	//                                  
	struct cmdline_t cmdline = parseCommandLine(argc, argv);

	if (cmdline.update) {
		return update(argv[0]);
	}

	//                        
	uid_t euid = geteuid();
	if (euid) {
		logg->logError(__FILE__, __LINE__, "gatord must be launched with root privileges");
		handleException();
	}

	//                                                                               
	if (!setupFilesystem(cmdline.module)) {
		logg->logMessage("Unable to setup gatorfs, trying perf");
		if (!gSessionData->perf.setup()) {
			logg->logError(__FILE__, __LINE__,
				       "Unable to locate gator.ko driver:\n"
				       "  >>> gator.ko should be co-located with gatord in the same directory\n"
				       "  >>> OR insmod gator.ko prior to launching gatord\n"
				       "  >>> OR specify the location of gator.ko on the command line\n"
				       "  >>> OR run Linux 3.4 or later with perf (CONFIG_PERF_EVENTS and CONFIG_HW_PERF_EVENTS) and tracing (CONFIG_TRACING and CONFIG_CONTEXT_SWITCH_TRACER) support to collect data via userspace only");
			handleException();
		}
	}

	{
		EventsXML eventsXML;
		mxml_node_t *xml = eventsXML.getTree();
		//                       
		for (Driver *driver = Driver::getHead(); driver != NULL; driver = driver->getNext()) {
			driver->readEvents(xml);
		}
		mxmlDelete(xml);
	}

	//                        
	signal(SIGCHLD, child_exit);

	//                                                                                                                      
	//                                                                                                                      
	signal(SIGPIPE, SIG_IGN);

	//                                                                             
	if (gSessionData->mSessionXMLPath) {
		child = new Child();
		child->run();
		delete child;
	} else {
		gSessionData->annotateListener.setup();
		sock = new OlyServerSocket(cmdline.port);
		udpListener.setup(cmdline.port);
		if (!monitor.init() ||
				!monitor.add(sock->getFd()) ||
				!monitor.add(udpListener.getReq()) ||
				!monitor.add(gSessionData->annotateListener.getFd()) ||
				false) {
			logg->logError(__FILE__, __LINE__, "Monitor setup failed");
			handleException();
		}
		//                                                      
		while (1) {
			struct epoll_event events[2];
			logg->logMessage("Waiting on connection...");
			int ready = monitor.wait(events, ARRAY_LENGTH(events), -1);
			if (ready < 0) {
				logg->logError(__FILE__, __LINE__, "Monitor::wait failed");
				handleException();
			}
			for (int i = 0; i < ready; ++i) {
				if (events[i].data.fd == sock->getFd()) {
					handleClient();
				} else if (events[i].data.fd == udpListener.getReq()) {
					udpListener.handle();
				} else if (events[i].data.fd == gSessionData->annotateListener.getFd()) {
					gSessionData->annotateListener.handle();
				}
			}
		}
	}

	cleanUp();
	return 0;
}
