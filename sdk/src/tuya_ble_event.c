
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_event.h"
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_sdk_version.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_log.h"


#if (!TUYA_BLE_USE_OS)

static  uint8_t       * m_queue_event_data;     
static volatile uint8_t m_queue_start_index;   
static volatile uint8_t m_queue_end_index;   
static uint16_t         m_queue_event_size;  
static uint16_t         m_queue_size;       


static __TUYA_BLE_INLINE uint8_t next_index(uint8_t index)
{
    return (index < m_queue_size) ? (index + 1) : 0;
}


static __TUYA_BLE_INLINE uint8_t tuya_sched_queue_full(void)
{
    uint8_t tmp = m_queue_start_index;
    return next_index(m_queue_end_index) == tmp;
}

#define TUYA_BLE_SCHED_QUEUE_FULL() tuya_sched_queue_full()


static __TUYA_BLE_INLINE uint8_t tuya_sched_queue_empty(void)
{
    uint8_t tmp = m_queue_start_index;
    return m_queue_end_index == tmp;
}


#define TUYA_BLE_SCHED_QUEUE_EMPTY() tuya_sched_queue_empty()


uint32_t tuya_ble_sched_init(uint16_t event_size, uint16_t queue_size, void * p_event_buffer)
{

    if (!tuya_ble_is_word_aligned_tuya(p_event_buffer))
    {
        TUYA_BLE_LOG_ERROR("tuya_ble_sched_init error");
        return 1;
    }

    m_queue_event_data    = &((uint8_t *)p_event_buffer)[0];
    m_queue_end_index     = 0;
    m_queue_start_index   = 0;
    m_queue_event_size    = event_size;
    m_queue_size          = queue_size;

    return 0;
}

uint16_t tuya_ble_sched_queue_size_get(void)
{
    return m_queue_size;
}

uint16_t tuya_ble_sched_queue_space_get(void)
{
    uint16_t start = m_queue_start_index;
    uint16_t end   = m_queue_end_index;
    uint16_t free_space = m_queue_size - ((end >= start) ?
                                          (end - start) : (m_queue_size + 1 - start + end));
    return free_space;
}


uint16_t tuya_ble_sched_queue_events_get(void)
{
    uint16_t start = m_queue_start_index;
    uint16_t end   = m_queue_end_index;
    uint16_t number_of_events;
    if(m_queue_size==0)
    {
        number_of_events = 0;
    }
    else
    {
        number_of_events = ((end >= start) ? (end - start) : (m_queue_size + 1 - start + end));
    }
    return number_of_events;
}


static tuya_ble_status_t tuya_ble_sched_event_put(void const  * p_event_data, uint16_t  event_data_size)
{
    tuya_ble_status_t err_code;

    if (event_data_size <= m_queue_event_size)
    {
        uint16_t event_index = 0xFFFF;

        tuya_ble_device_enter_critical();
		
        if (!TUYA_BLE_SCHED_QUEUE_FULL())
        {
            event_index       = m_queue_end_index;
            m_queue_end_index = next_index(m_queue_end_index);

        }
		
        tuya_ble_device_exit_critical();

        if (event_index != 0xFFFF)
        {

            if ((p_event_data != NULL) && (event_data_size > 0))
            {
                memcpy(&m_queue_event_data[event_index * m_queue_event_size],
                       p_event_data,
                       event_data_size);
                
            }
            else
            {

            }

            err_code = TUYA_BLE_SUCCESS;
        }
        else
        {
            err_code = TUYA_BLE_ERR_NO_MEM;
        }
    }
    else
    {
        err_code = TUYA_BLE_ERR_INVALID_LENGTH;
    }

    return err_code;
}


void tuya_sched_execute(void)
{
    static tuya_ble_evt_param_t tuya_ble_evt;
    tuya_ble_evt_param_t *evt;
    
    evt = &tuya_ble_evt;

    uint8_t end_ix = m_queue_end_index;

    while (m_queue_start_index != end_ix) //(!TUYA_BLE_SCHED_QUEUE_EMPTY()) 
    {
        uint16_t event_index = m_queue_start_index;

        void * p_event_data;

        p_event_data = &(m_queue_event_data[event_index * m_queue_event_size]);

        memcpy(evt,p_event_data,sizeof(tuya_ble_evt_param_t));
        
        //TUYA_BLE_LOG_DEBUG("TUYA_RECEIVE_EVT-0x%04x,start index-0x%04x,end index-0x%04x\n",evt->hdr.event,m_queue_start_index,m_queue_end_index);
                
        tuya_ble_event_process(evt);
        
        m_queue_start_index = next_index(m_queue_start_index);
    }

}


void tuya_ble_event_queue_init(void)
{
    if( (sizeof(tuya_ble_evt_param_t)) > TUYA_BLE_EVT_SIZE)
    {
        TUYA_BLE_LOG_ERROR("ERROR!!TUYA_BLE_EVT_SIZE is not enough!");
        return;
    }

    TUYA_BLE_SCHED_INIT(TUYA_BLE_EVT_SIZE, TUYA_BLE_EVT_MAX_NUM);
}


tuya_ble_status_t tuya_ble_message_send(tuya_ble_evt_param_t *evt)
{	
	return tuya_ble_sched_event_put(evt,m_queue_event_size);	
}


#endif


