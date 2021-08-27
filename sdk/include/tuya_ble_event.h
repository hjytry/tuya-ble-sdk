
#ifndef TUYA_BLE_EVENT_H_
#define TUYA_BLE_EVENT_H_

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"

#ifdef __cplusplus
extern "C" {
#endif


#if (!TUYA_BLE_USE_OS)


#define TUYA_BLE_EVT_MAX_NUM 		MAX_NUMBER_OF_TUYA_MESSAGE
#define TUYA_BLE_EVT_SIZE 		    52 //64   

enum
{
	TUYA_BLE_EVT_SEND_SUCCESS      = 0,
	TUYA_BLE_EVT_SEND_NO_MEMORY    = 1,
	TUYA_BLE_EVT_SEND_FAIL         = 2,
};

#define TUYA_BLE_ERROR_HANDLER(ERR_CODE)

#define TUYA_BLE_ERROR_CHECK(ERR_CODE)

#define TUYA_BLE_ERROR_CHECK_BOOL(BOOLEAN_VALUE)


#define CEIL_DIV(A, B)      \
    (((A) + (B) - 1) / (B))


#define TUYA_BLE_SCHED_BUF_SIZE(EVENT_SIZE, QUEUE_SIZE)                                                 \
            ((EVENT_SIZE) * ((QUEUE_SIZE) + 1))


#define TUYA_BLE_SCHED_INIT(EVENT_SIZE, QUEUE_SIZE)                                                     \
    do                                                                                             \
    {                                                                                              \
        static uint32_t TUYA_BLE_SCHED_BUF[CEIL_DIV(TUYA_BLE_SCHED_BUF_SIZE((EVENT_SIZE), (QUEUE_SIZE)),     \
                                               sizeof(uint32_t))];                                 \
        uint32_t ERR_CODE = tuya_ble_sched_init((EVENT_SIZE), (QUEUE_SIZE), TUYA_BLE_SCHED_BUF);            \
        TUYA_BLE_ERROR_CHECK(ERR_CODE);                                                                 \
    } while (0)                                                                                    

void tuya_sched_execute(void);

uint16_t tuya_ble_sched_queue_size_get(void); 
 
uint16_t tuya_ble_sched_queue_space_get(void);

uint16_t tuya_ble_sched_queue_events_get(void);

void tuya_ble_event_queue_init(void);

tuya_ble_status_t tuya_ble_message_send(tuya_ble_evt_param_t *evt);


#endif

#ifdef __cplusplus
}
#endif

#endif // 




