/**
  * @file    usb.c
  * @brief   Simple USB device implementation for STM32H503
  */

#include "usb_interface.h"

/* USB Device handle */
static USBD_HandleTypeDef USBD_Device;

/* USB descriptors */
static const uint8_t USBD_DeviceDesc[18] = {
  0x12,                       /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
  0x00, 0x02,                 /* bcdUSB = 2.00 */
  0xFF,                       /* bDeviceClass: Vendor Specific */
  0x00,                       /* bDeviceSubClass */
  0x00,                       /* bDeviceProtocol */
  USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
  (uint8_t)(USB_VID), (uint8_t)(USB_VID >> 8),       /* idVendor */
  (uint8_t)(USB_PID), (uint8_t)(USB_PID >> 8),       /* idProduct */
  0x00, 0x01,                 /* bcdDevice = 1.00 */
  0x01,                       /* iManufacturer */
  0x02,                       /* iProduct */
  0x03,                       /* iSerialNumber */
  0x01                        /* bNumConfigurations */
};

/* Configuration descriptor */
static const uint8_t USBD_ConfigDesc[25] = {
  /* Configuration Descriptor */
  0x09,                       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  25, 0x00,                   /* wTotalLength */
  0x01,                       /* bNumInterfaces */
  0x01,                       /* bConfigurationValue */
  0x00,                       /* iConfiguration */
  0x80,                       /* bmAttributes: Bus Powered */
  0x32,                       /* MaxPower 100 mA */

  /* Interface Descriptor */
  0x09,                       /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
  0x00,                       /* bInterfaceNumber */
  0x00,                       /* bAlternateSetting */
  0x00,                       /* bNumEndpoints */
  0xFF,                       /* bInterfaceClass: Vendor Specific */
  0x00,                       /* bInterfaceSubClass */
  0x00,                       /* bInterfaceProtocol */
  0x00                        /* iInterface */
};

/* Language ID Descriptor */
static const uint8_t USBD_LangIDDesc[4] = {
  0x04,                       /* bLength */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  0x09, 0x04                  /* wLANGID: 1033 (English) */
};

/* Manufacturer string */
static const uint8_t USBD_ManufacturerString[38] = {
  0x26,                       /* bLength */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0, 'l', 0,
  'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0, 'c', 0, 's', 0
};

/* Product string */
static const uint8_t USBD_ProductString[36] = {
  0x24,                       /* bLength */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  'V', 0, 'e', 0, 'n', 0, 'd', 0, 'o', 0, 'r', 0, ' ', 0, 'S', 0, 'p', 0,
  'e', 0, 'c', 0, 'i', 0, 'f', 0, 'i', 0, 'c', 0, ' ', 0, ' ', 0
};

/* Serial number string */
static const uint8_t USBD_SerialString[26] = {
  0x1A,                       /* bLength */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0,
  '0', 0, '0', 0, '0', 0, '1', 0
};

/* Private function prototypes */
static void USB_EP0_SetupStage(USBD_HandleTypeDef *pdev);
static void USB_EP0_DataInStage(USBD_HandleTypeDef *pdev);
static void USB_GetDescriptor(USBD_HandleTypeDef *pdev, USB_SetupReqTypedef *req);
static void USB_SetAddress(USBD_HandleTypeDef *pdev, USB_SetupReqTypedef *req);
static void USB_SetConfig(USBD_HandleTypeDef *pdev, USB_SetupReqTypedef *req);
static void USB_EP0_SendData(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len);
static void USB_EP0_SendStatus(USBD_HandleTypeDef *pdev);

/**
  * @brief  Initialize the USB device
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DeviceInit(PCD_HandleTypeDef *hpcd)
{
  /* Initialize USB Device handle */
  USBD_Device.pcd = hpcd;
  USBD_Device.state = USBD_STATE_DEFAULT;
  USBD_Device.address = 0;
  USBD_Device.configuration = 0;

  /* Set EP0 max packet size */
//  HAL_PCDEx_SetRxFiFo(hpcd, 128);
//  HAL_PCDEx_SetTxFiFo(hpcd, 0, 64);
  HAL_PCD_EP_Open(hpcd, 0x00, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);
  HAL_PCD_EP_Open(hpcd, 0x80, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);

  /* Start the USB device */
  HAL_PCD_Start(hpcd);

  return HAL_OK;
}

/**
  * @brief  Handle USB Reset event
  * @param  hpcd: PCD handle
  * @retval None
  */
void USB_ResetCallback(PCD_HandleTypeDef *hpcd)
{
  /* Reset device address */
  USBD_Device.address = 0;
  USBD_Device.state = USBD_STATE_DEFAULT;
  USBD_Device.configuration = 0;

  /* Open EP0 */
  HAL_PCD_EP_Open(hpcd, 0x00, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);
  HAL_PCD_EP_Open(hpcd, 0x80, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);
}

/**
  * @brief  Handle USB Setup stage
  * @param  hpcd: PCD handle
  * @retval None
  */
void USB_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_Device.pcd = hpcd;
  USB_EP0_SetupStage(&USBD_Device);
}

/**
  * @brief  Handle USB Data OUT stage
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void USB_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  if (epnum == 0) {
    /* EP0 OUT complete - status stage of control write */
    HAL_PCD_EP_SetStall(hpcd, 0x80);
  }
}

/**
  * @brief  Handle USB Data IN stage
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void USB_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  if (epnum == 0) {
    /* EP0 IN complete */
    USB_EP0_DataInStage(&USBD_Device);
  }
}

/**
  * @brief  Handle USB SOF event
  * @param  hpcd: PCD handle
  * @retval None
  */
void USB_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  /* Not used in this minimal implementation */
}

/**
  * @brief  Handle USB Suspend event
  * @param  hpcd: PCD handle
  * @retval None
  */
void USB_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_Device.state = USBD_STATE_SUSPENDED;
}

/**
  * @brief  Handle USB Resume event
  * @param  hpcd: PCD handle
  * @retval None
  */
void USB_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
  if (USBD_Device.state == USBD_STATE_SUSPENDED) {
    if (USBD_Device.address != 0) {
      USBD_Device.state = USBD_STATE_ADDRESSED;
    } else {
      USBD_Device.state = USBD_STATE_DEFAULT;
    }
  }
}

/**
  * @brief  Handle USB Connect event
  * @param  hpcd: PCD handle
  * @retval None
  */
void USB_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
  /* Not used in this minimal implementation */
}

/**
  * @brief  Handle USB Disconnect event
  * @param  hpcd: PCD handle
  * @retval None
  */
void USB_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_Device.state = USBD_STATE_DEFAULT;
  USBD_Device.address = 0;
  USBD_Device.configuration = 0;
}

/**
  * @brief  Process setup packet received on EP0
  * @param  pdev: USB device handle
  * @retval None
  */
static void USB_EP0_SetupStage(USBD_HandleTypeDef *pdev)
{
  USB_SetupReqTypedef *req = (USB_SetupReqTypedef *)pdev->setup_buffer;

  /* Copy setup packet data from PCD */
  HAL_PCD_EP_Receive(pdev->pcd, 0x00, pdev->setup_buffer, USB_FS_MAX_PACKET_SIZE);

  /* Process standard requests */
  if ((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
    switch (req->bRequest) {
      case USB_REQ_GET_DESCRIPTOR:
        USB_GetDescriptor(pdev, req);
        break;

      case USB_REQ_SET_ADDRESS:
        USB_SetAddress(pdev, req);
        break;

      case USB_REQ_SET_CONFIGURATION:
        USB_SetConfig(pdev, req);
        break;

      case USB_REQ_GET_STATUS:
        if (req->wLength == 2) {
          pdev->ep0_data_buffer[0] = 0x00;
          pdev->ep0_data_buffer[1] = 0x00;
          USB_EP0_SendData(pdev, pdev->ep0_data_buffer, 2);
        } else {
          HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
        }
        break;

      default:
        HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
        HAL_PCD_EP_SetStall(pdev->pcd, 0x00);
        break;
    }
  } else if ((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
    /* Handle vendor-specific requests here if needed */
    /* For this minimal implementation, just stall unhandled requests */
    HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
    HAL_PCD_EP_SetStall(pdev->pcd, 0x00);
  } else {
    /* Stall unhandled requests */
    HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
    HAL_PCD_EP_SetStall(pdev->pcd, 0x00);
  }
}

/**
  * @brief  Process data in stage on EP0
  * @param  pdev: USB device handle
  * @retval None
  */
static void USB_EP0_DataInStage(USBD_HandleTypeDef *pdev)
{
  /* Check if address was assigned */
  if (pdev->state == USBD_STATE_DEFAULT && pdev->address > 0) {
    /* Set the device address */
    HAL_PCD_SetAddress(pdev->pcd, pdev->address);
    pdev->state = USBD_STATE_ADDRESSED;
  }
}

/**
  * @brief  Process GET_DESCRIPTOR request
  * @param  pdev: USB device handle
  * @param  req: Setup request
  * @retval None
  */
static void USB_GetDescriptor(USBD_HandleTypeDef *pdev, USB_SetupReqTypedef *req)
{
  uint8_t desc_type = (uint8_t)(req->wValue >> 8);
  uint8_t desc_idx = (uint8_t)(req->wValue & 0xFF);
  uint16_t len = 0;
  const uint8_t *pbuf = NULL;

  switch (desc_type) {
    case USB_DEVICE_DESCRIPTOR_TYPE:
      pbuf = USBD_DeviceDesc;
      len = 18;
      break;

    case USB_CONFIGURATION_DESCRIPTOR_TYPE:
      pbuf = USBD_ConfigDesc;
      len = 25;
      break;

    case USB_STRING_DESCRIPTOR_TYPE:
      switch (desc_idx) {
        case 0:  /* Language ID */
          pbuf = USBD_LangIDDesc;
          len = 4;
          break;

        case 1:  /* Manufacturer */
          pbuf = USBD_ManufacturerString;
          len = USBD_ManufacturerString[0];
          break;

        case 2:  /* Product */
          pbuf = USBD_ProductString;
          len = USBD_ProductString[0];
          break;

        case 3:  /* Serial Number */
          pbuf = USBD_SerialString;
          len = USBD_SerialString[0];
          break;

        default:
          HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
          return;
      }
      break;

    default:
      HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
      return;
  }

  /* Send the descriptor */
  if (len > 0 && pbuf != NULL) {
    len = (len < req->wLength) ? len : req->wLength;
    USB_EP0_SendData(pdev, (uint8_t *)pbuf, len);
  } else {
    HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
  }
}

/**
  * @brief  Process SET_ADDRESS request
  * @param  pdev: USB device handle
  * @param  req: Setup request
  * @retval None
  */
static void USB_SetAddress(USBD_HandleTypeDef *pdev, USB_SetupReqTypedef *req)
{
  /* Store the new address */
  pdev->address = (uint8_t)(req->wValue & 0x7F);

  /* Send zero-length packet for status stage */
  USB_EP0_SendStatus(pdev);
}

/**
  * @brief  Process SET_CONFIGURATION request
  * @param  pdev: USB device handle
  * @param  req: Setup request
  * @retval None
  */
static void USB_SetConfig(USBD_HandleTypeDef *pdev, USB_SetupReqTypedef *req)
{
  uint8_t config = (uint8_t)(req->wValue & 0xFF);

  if (config == 0) {
    /* Set device to address state */
    pdev->state = USBD_STATE_ADDRESSED;
    pdev->configuration = 0;
  } else if (config == 1) {
    /* Set configuration and move to configured state */
    pdev->configuration = 1;
    pdev->state = USBD_STATE_CONFIGURED;
  } else {
    /* Invalid configuration value */
    HAL_PCD_EP_SetStall(pdev->pcd, 0x80);
    return;
  }

  /* Send status stage */
  USB_EP0_SendStatus(pdev);
}

/**
  * @brief  Send data on EP0 IN
  * @param  pdev: USB device handle
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval None
  */
static void USB_EP0_SendData(USBD_HandleTypeDef *pdev, uint8_t *buf, uint16_t len)
{
  /* Store data for transmission */
  pdev->ep0_data_length = len;

  /* Copy data to buffer if needed */
  if (buf != pdev->ep0_data_buffer) {
    for (uint16_t i = 0; i < len; i++) {
      pdev->ep0_data_buffer[i] = buf[i];
    }
  }

  /* Transmit the data */
  HAL_PCD_EP_Transmit(pdev->pcd, 0x80, pdev->ep0_data_buffer, len);
}

/**
  * @brief  Send zero-length packet on EP0 IN (status stage)
  * @param  pdev: USB device handle
  * @retval None
  */
static void USB_EP0_SendStatus(USBD_HandleTypeDef *pdev)
{
  HAL_PCD_EP_Transmit(pdev->pcd, 0x80, NULL, 0);
}
