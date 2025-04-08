/**
  * @file    usb.h
  * @brief   Simple USB device implementation for STM32H503
  */

#ifndef __USB_H
#define __USB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h5xx_hal.h"
#include "stm32h5xx_ll_usb.h"

/* USB Vendor ID and Product ID */
#define USB_VID                     0x0483  /* ST Microelectronics VID */
#define USB_PID                     0x5750  /* Example PID for vendor device */

/* USB Device descriptor parameters */
#define USB_DEVICE_DESCRIPTOR_TYPE  0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE 0x02
#define USB_STRING_DESCRIPTOR_TYPE  0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE 0x05

#define USB_MAX_EP0_SIZE            64
#define USB_FS_MAX_PACKET_SIZE      64

/* USB Standard Request Codes */
#define USB_REQ_GET_STATUS          0x00
#define USB_REQ_CLEAR_FEATURE       0x01
#define USB_REQ_SET_FEATURE         0x03
#define USB_REQ_SET_ADDRESS         0x05
#define USB_REQ_GET_DESCRIPTOR      0x06
#define USB_REQ_SET_DESCRIPTOR      0x07
#define USB_REQ_GET_CONFIGURATION   0x08
#define USB_REQ_SET_CONFIGURATION   0x09
#define USB_REQ_GET_INTERFACE       0x0A
#define USB_REQ_SET_INTERFACE       0x0B

/* bmRequestType bit definitions */
#define USB_REQ_TYPE_MASK           0x60
#define USB_REQ_TYPE_STANDARD       0x00
#define USB_REQ_TYPE_CLASS          0x20
#define USB_REQ_TYPE_VENDOR         0x40

#define USB_REQ_RECIPIENT_MASK      0x1F
#define USB_REQ_RECIPIENT_DEVICE    0x00
#define USB_REQ_RECIPIENT_INTERFACE 0x01
#define USB_REQ_RECIPIENT_ENDPOINT  0x02

#define USB_REQ_DIR_MASK            0x80
#define USB_REQ_DIR_H2D             0x00
#define USB_REQ_DIR_D2H             0x80

/* Device states */
typedef enum {
  USBD_STATE_DEFAULT    = 0x00,
  USBD_STATE_ADDRESSED  = 0x01,
  USBD_STATE_CONFIGURED = 0x02,
  USBD_STATE_SUSPENDED  = 0x03
} USBD_StateTypeDef;

/* Setup packet structure */
typedef struct {
  uint8_t   bmRequestType;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
} USB_SetupReqTypedef;

/* USB Device handle structure */
typedef struct {
  PCD_HandleTypeDef     *pcd;
  uint8_t               address;
  USBD_StateTypeDef     state;
  uint8_t               configuration;
  uint8_t               setup_buffer[8];
  uint8_t               ep0_data_buffer[USB_MAX_EP0_SIZE];
  uint16_t              ep0_data_length;
} USBD_HandleTypeDef;

/* Exported functions */
HAL_StatusTypeDef USB_DeviceInit(PCD_HandleTypeDef *hpcd);
void USB_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void USB_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void USB_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void USB_SOFCallback(PCD_HandleTypeDef *hpcd);
void USB_ResetCallback(PCD_HandleTypeDef *hpcd);
void USB_SuspendCallback(PCD_HandleTypeDef *hpcd);
void USB_ResumeCallback(PCD_HandleTypeDef *hpcd);
void USB_ConnectCallback(PCD_HandleTypeDef *hpcd);
void USB_DisconnectCallback(PCD_HandleTypeDef *hpcd);

#ifdef __cplusplus
}
#endif

#endif /* __USB_H */
