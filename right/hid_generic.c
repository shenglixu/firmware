#include "include/board/board.h"
#include "fsl_gpio.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "include/usb/usb_device_class.h"
#include "include/usb/usb_device_hid.h"
#include "include/usb/usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "composite.h"
#include "scancodes.h"
#include "hid_generic.h"

static usb_device_generic_hid_struct_t UsbGenericHidDevice;

usb_status_t UsbGenericHidCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            break;
        case kUSB_DeviceHidEventRecvResponse:
            GPIO_SetPinsOutput(BOARD_LED_RED_GPIO, 1U << BOARD_LED_RED_GPIO_PIN);
            GPIO_SetPinsOutput(BOARD_LED_GREEN_GPIO, 1U << BOARD_LED_GREEN_GPIO_PIN);
            GPIO_SetPinsOutput(BOARD_LED_BLUE_GPIO, 1U << BOARD_LED_BLUE_GPIO_PIN);

            uint8_t* cp = (uint8_t*)&UsbGenericHidDevice.buffer[UsbGenericHidDevice.bufferIndex][0];
            uint8_t c = *(cp+0);
            if (c == 'r') {
                GPIO_ClearPinsOutput(BOARD_LED_RED_GPIO, 1U << BOARD_LED_RED_GPIO_PIN);
            }
            if (c == 'g') {
                GPIO_ClearPinsOutput(BOARD_LED_GREEN_GPIO, 1U << BOARD_LED_GREEN_GPIO_PIN);
            }
            if (c == 'b') {
                GPIO_ClearPinsOutput(BOARD_LED_BLUE_GPIO, 1U << BOARD_LED_BLUE_GPIO_PIN);
            }

            USB_DeviceHidSend(UsbCompositeDevice.genericHidHandle, USB_GENERIC_HID_ENDPOINT_IN_ID,
                              (uint8_t *)&UsbGenericHidDevice.buffer[UsbGenericHidDevice.bufferIndex][0],
                              USB_GENERIC_HID_OUT_BUFFER_LENGTH);
            UsbGenericHidDevice.bufferIndex ^= 1U;
            return USB_DeviceHidRecv(UsbCompositeDevice.genericHidHandle, USB_GENERIC_HID_ENDPOINT_OUT_ID,
                                     (uint8_t *)&UsbGenericHidDevice.buffer[UsbGenericHidDevice.bufferIndex][0],
                                     USB_GENERIC_HID_OUT_BUFFER_LENGTH);
            break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        case kUSB_DeviceHidEventRequestReportBuffer:
            error = kStatus_USB_InvalidRequest;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            break;
        default:
            break;
    }

    return error;
}
usb_status_t UsbGenericHidSetConfiguration(class_handle_t handle, uint8_t configuration)
{
    if (USB_COMPOSITE_CONFIGURATION_INDEX == configuration) {
        return USB_DeviceHidRecv(
                UsbCompositeDevice.genericHidHandle, USB_GENERIC_HID_ENDPOINT_OUT_ID,
            (uint8_t *)&UsbGenericHidDevice.buffer[UsbGenericHidDevice.bufferIndex][0],
            USB_GENERIC_HID_OUT_BUFFER_LENGTH);
    }
    return kStatus_USB_Error;
}

usb_status_t UsbGenericHidSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_KEYBOARD_INTERFACE_INDEX == interface) {
        UsbCompositeDevice.currentInterfaceAlternateSetting[interface] = alternateSetting;
        if (alternateSetting == 0U)
        {
            return USB_DeviceHidRecv(
                    UsbCompositeDevice.genericHidHandle, USB_GENERIC_HID_ENDPOINT_OUT_ID,
                (uint8_t *)&UsbGenericHidDevice.buffer[UsbGenericHidDevice.bufferIndex][0],
                USB_GENERIC_HID_OUT_BUFFER_LENGTH);
        }
    }
    return kStatus_USB_Error;
}