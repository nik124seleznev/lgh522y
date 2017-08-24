/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





/*                                                                
                                                                  
                                                                   */

/*
                              
  
                                                                     
                                                                             
                                                                         
                                                                          
                                           
                                                                           
                                                           
                                                                  
                               
                    
                    
                     
                     
                    
                   
                               
                      
 */

/*
                                  
                                                                             
                       
                                                                           
  
         
                                                                               
                                                                         
                                                          
  
                                                                              
                                
                                                                               
                                                          
                                                                       
                                                                             
                                   
 */

	/*
              
  */
	/*                                               */
	KBASE_TIMELINE_TRACE_CODE(CTX_SET_NR_ATOMS_IN_FLIGHT,     "CTX: Atoms in flight",            "%d,%d",    "_instance_tgid,_value_number_of_atoms"),
	KBASE_TIMELINE_TRACE_CODE(CTX_FLOW_ATOM_READY,            "CTX: Atoms Ready to Run",         "%d,%d,%d", "_instance_tgid,_consumerof_atom_number,_producerof_atom_number_ready"),

	/*
             
  */
	/*                                             */
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_SLOT_ACTIVE,         "SW: GPU slot active",             "%d,%d,%d", "_tgid,_instance_slot,_value_number_of_atoms"),
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_SLOT_NEXT,           "SW: GPU atom in NEXT",            "%d,%d,%d", "_tgid,_instance_slot,_value_is_an_atom_in_next"),
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_SLOT_HEAD,           "SW: GPU atom in HEAD",            "%d,%d,%d", "_tgid,_instance_slot,_value_is_an_atom_in_head"),
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_SLOT_STOPPING,       "SW: Try Soft-Stop on GPU slot",   "%d,%d,%d", "_tgid,_instance_slot,_value_is_slot_stopping"),
	/*                                                                      
                                      */
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_POWER_ACTIVE,        "SW: GPU power active",            "%d,%d",    "_tgid,_value_is_power_active"),
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_POWER_TILER_ACTIVE,  "SW: GPU tiler powered",           "%d,%d",    "_tgid,_value_number_of_tilers"),
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_POWER_SHADER_ACTIVE, "SW: GPU shaders powered",         "%d,%d",    "_tgid,_value_number_of_shaders"),
	KBASE_TIMELINE_TRACE_CODE(SW_SET_GPU_POWER_L2_ACTIVE,     "SW: GPU L2 powered",              "%d,%d",    "_tgid,_value_number_of_l2"),

	/*                                                                            */
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_SEND_EVENT,          "SW: PM Send Event",               "%d,%d,%d", "_tgid,_event_type,_writerof_pm_event_id"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_HANDLE_EVENT,        "SW: PM Handle Event",             "%d,%d,%d", "_tgid,_event_type,_finalconsumerof_pm_event_id"),
	/*                    */
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_GPU_POWER_L2_POWERING,  "SW: GPU L2 powering",             "%d,%d", "_tgid,_writerof_l2_transitioning"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_GPU_POWER_L2_ACTIVE,	  "SW: GPU L2 powering done",        "%d,%d", "_tgid,_finalconsumerof_l2_transitioning"),

	KBASE_TIMELINE_TRACE_CODE(SW_SET_CONTEXT_ACTIVE,          "SW: Context Active",              "%d,%d",    "_tgid,_value_active"),

	/*
                                                                                 
  */
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_DO_POWEROFF_START, "SW: PM CheckTrans from kbase_pm_do_poweroff", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_do_poweroff"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_DO_POWEROFF_END,   "SW: PM CheckTrans from kbase_pm_do_poweroff", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_do_poweroff"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_DO_POWERON_START, "SW: PM CheckTrans from kbase_pm_do_poweron", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_do_poweron"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_DO_POWERON_END,   "SW: PM CheckTrans from kbase_pm_do_poweron", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_do_poweron"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_GPU_INTERRUPT_START, "SW: PM CheckTrans from kbase_gpu_interrupt", "%d,%d", "_tgid,_writerof_pm_checktrans_gpu_interrupt"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_GPU_INTERRUPT_END,   "SW: PM CheckTrans from kbase_gpu_interrupt", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_gpu_interrupt"),

	/*
                                                                       
  */
	/*                        */
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_START, "SW: PM CheckTrans from kbase_pm_request_cores(shader)", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_request_cores_shader"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_END,   "SW: PM CheckTrans from kbase_pm_request_cores(shader)", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_request_cores_shader"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_TILER_START, "SW: PM CheckTrans from kbase_pm_request_cores(tiler)", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_request_cores_tiler"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_TILER_END,   "SW: PM CheckTrans from kbase_pm_request_cores(tiler)", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_request_cores_tiler"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_TILER_START, "SW: PM CheckTrans from kbase_pm_request_cores(shader+tiler)", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_request_cores_shader_tiler"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_TILER_END,   "SW: PM CheckTrans from kbase_pm_request_cores(shader+tiler)", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_request_cores_shader_tiler"),
	/*                        */
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_START, "SW: PM CheckTrans from kbase_pm_release_cores(shader)", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_release_cores_shader"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_END,   "SW: PM CheckTrans from kbase_pm_release_cores(shader)", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_release_cores_shader"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_TILER_START, "SW: PM CheckTrans from kbase_pm_release_cores(tiler)", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_release_cores_tiler"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_TILER_END,   "SW: PM CheckTrans from kbase_pm_release_cores(tiler)", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_release_cores_tiler"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_TILER_START, "SW: PM CheckTrans from kbase_pm_release_cores(shader+tiler)", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_release_cores_shader_tiler"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_TILER_END,   "SW: PM CheckTrans from kbase_pm_release_cores(shader+tiler)", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_release_cores_shader_tiler"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_DEFERRED_START, "SW: PM CheckTrans from kbasep_pm_do_shader_poweroff_callback", "%d,%d", "_tgid,_writerof_pm_checktrans_pm_do_shader_poweroff_callback"),
	KBASE_TIMELINE_TRACE_CODE(SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_DEFERRED_END,   "SW: PM CheckTrans from kbasep_pm_do_shader_poweroff_callback", "%d,%d", "_tgid,_finalconsumerof_pm_checktrans_pm_do_shader_poweroff_callback"),
	/*
                                                                   
  */

	/*
             
  */
	KBASE_TIMELINE_TRACE_CODE(HW_START_GPU_JOB_CHAIN_SW_APPROX,     "HW: Job Chain start (SW approximated)", "%d,%d,%d", "_tgid,job_slot,_consumerof_atom_number_ready"),
	KBASE_TIMELINE_TRACE_CODE(HW_STOP_GPU_JOB_CHAIN_SW_APPROX,      "HW: Job Chain stop (SW approximated)",  "%d,%d,%d", "_tgid,job_slot,_producerof_atom_number_completed")
