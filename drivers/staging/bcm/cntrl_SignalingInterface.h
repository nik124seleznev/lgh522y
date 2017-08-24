#ifndef CNTRL_SIGNALING_INTERFACE_
#define CNTRL_SIGNALING_INTERFACE_

#define DSA_REQ			11
#define DSA_RSP			12
#define DSA_ACK			13
#define DSC_REQ			14
#define DSC_RSP			15
#define DSC_ACK			16
#define DSD_REQ			17
#define DSD_RSP			18
#define DSD_ACK			19
#define MAX_CLASSIFIERS_IN_SF	4

#define MAX_STRING_LEN			20
#define MAX_PHS_LENGTHS			255
#define VENDOR_PHS_PARAM_LENGTH		10
#define MAX_NUM_ACTIVE_BS		10
#define AUTH_TOKEN_LENGTH		10
#define NUM_HARQ_CHANNELS		16 /*                                                        */
#define VENDOR_CLASSIFIER_PARAM_LENGTH	1  /*                                                */
#define  VENDOR_SPECIF_QOS_PARAM	1
#define VENDOR_PHS_PARAM_LENGTH		10
#define MBS_CONTENTS_ID_LENGTH		10
#define GLOBAL_SF_CLASSNAME_LENGTH	6

#define TYPE_OF_SERVICE_LENGTH		3
#define IP_MASKED_SRC_ADDRESS_LENGTH	32
#define IP_MASKED_DEST_ADDRESS_LENGTH	32
#define PROTOCOL_SRC_PORT_RANGE_LENGTH	4
#define PROTOCOL_DEST_PORT_RANGE_LENGTH	4
#define ETHERNET_DEST_MAC_ADDR_LENGTH	12
#define ETHERNET_SRC_MAC_ADDR_LENGTH	12
#define NUM_ETHERTYPE_BYTES		3
#define NUM_IPV6_FLOWLABLE_BYTES	3

struct bcm_packet_class_rules {
	/*                                        */
	u16 u16UserPriority;
	/*                                  */
	u16 u16VLANID;
	/*                                                           */
	u16 u16PacketClassificationRuleIndex;
	/*                                                   */
	u8 u8ClassifierRulePriority;
	/*                                  */
	u8 u8IPTypeOfServiceLength;
	/*                         */
	u8 u8IPTypeOfService[TYPE_OF_SERVICE_LENGTH];
	/*                                                 */
	u8 u8Protocol;
	/*                                    */
	u8 u8IPMaskedSourceAddressLength;
	/*                                                                      */
	u8 u8IPMaskedSourceAddress[IP_MASKED_SRC_ADDRESS_LENGTH];
	/*                                  */
	u8 u8IPDestinationAddressLength;
	/*                                                                    */
	u8 u8IPDestinationAddress[IP_MASKED_DEST_ADDRESS_LENGTH];
	/*                                      */
	u8 u8ProtocolSourcePortRangeLength;
	/*                                                     */
	u8 u8ProtocolSourcePortRange[PROTOCOL_SRC_PORT_RANGE_LENGTH];
	/*                                    */
	u8 u8ProtocolDestPortRangeLength;
	/*                                                   */
	u8 u8ProtocolDestPortRange[PROTOCOL_DEST_PORT_RANGE_LENGTH];
	/*                                            */
	u8 u8EthernetDestMacAddressLength;
	/*                                                                              */
	u8 u8EthernetDestMacAddress[ETHERNET_DEST_MAC_ADDR_LENGTH];
	/*                                       */
	u8 u8EthernetSourceMACAddressLength;
	/*                                                                         */
	u8 u8EthernetSourceMACAddress[ETHERNET_SRC_MAC_ADDR_LENGTH];
	/*                     */
	u8 u8EthertypeLength;
	/*                                      */
	u8 u8Ethertype[NUM_ETHERTYPE_BYTES];
	/*                                          */
	u8 u8AssociatedPHSI;
	/*                                                                       */
	u8 u8VendorSpecificClassifierParamLength;
	/*                                                      */
	u8 u8VendorSpecificClassifierParam[VENDOR_CLASSIFIER_PARAM_LENGTH];
	/*                                               */
	u8 u8IPv6FlowLableLength;
	/*                                     */
	u8 u8IPv6FlowLable[NUM_IPV6_FLOWLABLE_BYTES];
	/*                                            */
	u8 u8ClassifierActionRule;
	u16 u16ValidityBitMap;
};

struct bcm_phs_rules {
	/*                                    */
	u8 u8PHSI;
	/*                                 */
	u8 u8PHSFLength;
	/*                                                                                                                        */
	u8 u8PHSF[MAX_PHS_LENGTHS];
	/*                                 */
	u8 u8PHSMLength;
	/*                     */
	u8 u8PHSM[MAX_PHS_LENGTHS];
	/*                                                                  */
	u8 u8PHSS;
	/*                                                                                               */
	u8 u8PHSV;
	/*                                                      */
	u8 u8VendorSpecificPHSParamsLength;
	/*                                               */
	u8 u8VendorSpecificPHSParams[VENDOR_PHS_PARAM_LENGTH];
	u8 u8Padding[2];
};

struct bcm_convergence_types {
	/*                                               */
	u8 u8ClassfierDSCAction;
	/*                                         */
	u8 u8PhsDSCAction;
	/*               */
	u8 u8Padding[2];
	/*                                       */
	struct bcm_packet_class_rules cCPacketClassificationRule;
	/*                                            */
	struct bcm_phs_rules cPhsRule;
};

struct bcm_connect_mgr_params {
	/*                               */
	u32 u32SFID;
	/*                                                          */
	u32 u32MaxSustainedTrafficRate;
	/*                                                          */
	u32 u32MaxTrafficBurst;
	/*                                                         */
	u32 u32MinReservedTrafficRate;
	/*                                            */
	u32 u32ToleratedJitter;
	/*                                           */
	u32 u32MaximumLatency;
	/*                              */
	u16 u16CID;
	/*                                                                   */
	u16 u16TargetSAID;
	/*                                   */
	u16 u16ARQWindowSize;
	/*                                                                  */
	u16 u16ARQRetryTxTimeOut;
	/*                                                                  */
	u16 u16ARQRetryRxTimeOut;
	/*                          */
	u16 u16ARQBlockLifeTime;
	/*                             */
	u16 u16ARQSyncLossTimeOut;
	/*                         */
	u16 u16ARQRxPurgeTimeOut;
	/*                                                     
                              
  */
	u16 u16ARQBlockSize;
	/*        */
	/*                                                                */
	u16 u16SDUInterArrivalTime;
	/*                                                    */
	u16 u16TimeBase;
	/*                                                   */
	u16 u16UnsolicitedGrantInterval;
	/*                                                           */
	u16 u16UnsolicitedPollingInterval;
	/*                                  */
	u16 u16MacOverhead;
	/*                         */
	u16 u16MBSContentsID[MBS_CONTENTS_ID_LENGTH];
	/*                                */
	u8 u8MBSContentsIDLength;
	/*                                             */
	u8 u8ServiceClassNameLength;
	/*                                              */
	u8 u8ServiceClassName[32];
	/*                                                                              */
	u8 u8MBSService;
	/*                                                                                                                */
	u8 u8QosParamSet;
	/*                                           */
	u8 u8TrafficPriority;
	/*                                                       */
	u8 u8ServiceFlowSchedulingType;
	/*                                                      */
	u8 u8RequesttransmissionPolicy;
	/*                                                                                         */
	u8 u8FixedLengthVSVariableLengthSDUIndicator;
	/*                                                            */
	u8 u8SDUSize;
	/*                                                                    */
	u8 u8ARQEnable;
	/*                                                                                  */
	u8 u8ARQDeliverInOrder;
	/*                                       */
	u8 u8RxARQAckProcessingTime;
	/*                                                             */
	u8 u8CSSpecification;
	/*                                     */
	u8 u8TypeOfDataDeliveryService;
	/*                                                           */
	u8 u8PagingPreference;
	/*                                                                                         */
	u8 u8MBSZoneIdentifierassignment;
	/*                                                                                      */
	u8 u8TrafficIndicationPreference;
	/*                                                                                       */
	u8 u8GlobalServicesClassNameLength;
	/*                                                                               */
	u8 u8GlobalServicesClassName[GLOBAL_SF_CLASSNAME_LENGTH];
	/*                                                                   */
	u8 u8SNFeedbackEnabled;
	/*                                                                       */
	u8 u8FSNSize;
	/*                                       */
	u8 u8CIDAllocation4activeBSsLength;
	/*                               */
	u8 u8CIDAllocation4activeBSs[MAX_NUM_ACTIVE_BS];
	/*                                                                                 */
	u8 u8PDUSNExtendedSubheader4HarqReordering;
	/*                                                        */
	u8 u8HARQServiceFlows;
	/*                                             */
	u8 u8AuthTokenLength;
	/*                                   */
	u8 u8AuthToken[AUTH_TOKEN_LENGTH];
	/*                                                            */
	u8 u8HarqChannelMappingLength;
	/*                                           */
	u8 u8HARQChannelMapping[NUM_HARQ_CHANNELS];
	/*                                           */
	u8 u8VendorSpecificQoSParamLength;
	/*                                                      */
	u8 u8VendorSpecificQoSParam[VENDOR_SPECIF_QOS_PARAM];
	/*                                       */
	u8 u8TotalClassifiers;  /*                                     */
	u8 bValid;	/*                 */
	u8 u8Padding;	 /*                */
	/*
                                                                            
  */
	struct bcm_convergence_types cConvergenceSLTypes[MAX_CLASSIFIERS_IN_SF];
};

struct bcm_add_request {
	u8 u8Type;	/*        */
	u8 eConnectionDir; /*                        */
	/*                  */
	u16 u16TID; /*             */
	/*                */
	u16 u16CID; /*             */
	/*                 */
	u16 u16VCID; /*              */
	struct bcm_connect_mgr_params *psfParameterSet; /*                                 */
};

struct bcm_add_indication {
	u8 u8Type;	/*        */
	u8 eConnectionDir;	/*                        */
	/*                  */
	u16 u16TID; /*       */
	/*                */
	u16 u16CID; /*            */
	/*                 */
	u16 u16VCID; /*             */
	struct bcm_connect_mgr_params *psfAuthorizedSet; /*                                                 */
	struct bcm_connect_mgr_params *psfAdmittedSet; /*                                               */
	struct bcm_connect_mgr_params *psfActiveSet; /*                                            */
	u8 u8CC; /*                    */
	u8 u8Padd; /*                 */
	u16 u16Padd; /*                  */
};

struct bcm_del_request {
	u8 u8Type; /*        */
	u8 u8Padding; /*                */
	u16 u16TID; /*       */
	/*                 */
	u32 u32SFID; /*        */
};

struct bcm_del_indication {
	u8 u8Type;	/*        */
	u8 u8Padding; /*           */
	u16 u16TID; /*       */
	/*                */
	u16 u16CID; /*       */
	/*                 */
	u16 u16VCID; /*        */
	/*                 */
	u32 u32SFID; /*        */
	/*                              */
	u8 u8ConfirmationCode; /*                     */
	u8 u8Padding1[3]; /*                  */
};

struct bcm_stim_sfhostnotify {
	u32 SFID; /*                          */
	u16 newCID; /*                     */
	u16 VCID; /*                                                                                                            */
	u8 RetainSF; /*                                                                                       */
	u8 QoSParamSet; /*                                 */
	u16 u16reserved; /*                    */
};

#endif
