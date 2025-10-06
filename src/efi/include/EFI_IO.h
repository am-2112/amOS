#ifndef _EFI_IO_
#define _EFI_IO_

#include "efi_structs.h"
#include "efi_funcs.h"

INTERFACE_DECL(_SIMPLE_INPUT_INTERFACE);
INTERFACE_DECL(_SIMPLE_TEXT_OUTPUT_INTERFACE);
INTERFACE_DECL(_EFI_USBFN_IO_PROTOCOL);
INTERFACE_DECL(_EFI_USB2_HC_PROTOCOL);
INTERFACE_DECL(_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL);
INTERFACE_DECL(_EFI_FILE_PROTOCOL);

typedef struct {
    UINT16                              ScanCode;
    CHAR16                              UnicodeChar;
} EFI_INPUT_KEY;

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET) (
    IN struct _SIMPLE_INPUT_INTERFACE   *This,
    IN BOOLEAN                          ExtendedVerification
    );

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY) (
    IN struct _SIMPLE_INPUT_INTERFACE   *This,
    OUT EFI_INPUT_KEY                   *Key
    );

typedef struct _SIMPLE_INPUT_INTERFACE {
    EFI_INPUT_RESET                     Reset;
    EFI_INPUT_READ_KEY                  ReadKeyStroke;
    EFI_EVENT                           WaitForKey;
} SIMPLE_INPUT_INTERFACE, EFI_SIMPLE_TEXT_IN_PROTOCOL;



typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_RESET) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN BOOLEAN                      ExtendedVerification
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_OUTPUT_STRING) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN CHAR16                       *String
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_TEST_STRING) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN CHAR16                       *String
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_QUERY_MODE) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN UINTN                        ModeNumber,
    OUT UINTN                       *Columns,
    OUT UINTN                       *Rows
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_MODE) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN UINTN                        ModeNumber
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_ATTRIBUTE) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN UINTN                        Attribute
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_CLEAR_SCREEN) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_CURSOR_POSITION) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN UINTN                        Column,
    IN UINTN                        Row
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_ENABLE_CURSOR) (
    IN struct _SIMPLE_TEXT_OUTPUT_INTERFACE     *This,
    IN BOOLEAN                      Enable
    );

typedef struct {
    INT32                           MaxMode;
    // current settings
    INT32                           Mode;
    INT32                           Attribute;
    INT32                           CursorColumn;
    INT32                           CursorRow;
    BOOLEAN                         CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

typedef struct _SIMPLE_TEXT_OUTPUT_INTERFACE {
    EFI_TEXT_RESET                  Reset;

    EFI_TEXT_OUTPUT_STRING          OutputString;
    EFI_TEXT_TEST_STRING            TestString;

    EFI_TEXT_QUERY_MODE             QueryMode;
    EFI_TEXT_SET_MODE               SetMode;
    EFI_TEXT_SET_ATTRIBUTE          SetAttribute;

    EFI_TEXT_CLEAR_SCREEN           ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION    SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR          EnableCursor;

    // Current mode
    SIMPLE_TEXT_OUTPUT_MODE         *Mode;
} SIMPLE_TEXT_OUTPUT_INTERFACE, EFI_SIMPLE_TEXT_OUT_PROTOCOL;



/*USB IO*/


typedef enum _EFI_USBFN_PORT_TYPE {
	EfiUsbUnknownPort = 0,
	EfiUsbStandardDownstreamPort,
	EfiUsbChargingDownstreamPort,
	EfiUsbDedicatedChargingPort,
	EfiUsbInvalidDedicatedChargingPort
} EFI_USBFN_PORT_TYPE;

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_DETECT_PORT) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	OUT EFI_USBFN_PORT_TYPE *PortType
	);
	
typedef struct {
	UINT8 Length;
	UINT8 DescriptorType;
	UINT8 InterfaceNumber;
	UINT8 AlternateSetting;
	UINT8 NumEndpoints;
	UINT8 InterfaceClass;
	UINT8 InterfaceSubClass;
	UINT8 InterfaceProtocol;
	UINT8 Interface;
} EFI_USB_INTERFACE_DESCRIPTOR;
	
typedef struct {
	UINT8 Length;
	UINT8 DescriptorType;
	UINT8 EndpointAddress;
	UINT8 Attributes;
	UINT8 MaxPacketSize;
	UINT8 Interval;
} EFI_USB_ENDPOINT_DESCRIPTOR;
	
typedef struct {
	EFI_USB_INTERFACE_DESCRIPTOR *InterfaceDescriptor;
	EFI_USB_ENDPOINT_DESCRIPTOR **EndpointDescriptorTable;
} EFI_USB_INTERFACE_INFO;

typedef struct {
	UINT8 Length;
	UINT8 DescriptorType;
	UINT16 TotalLength;
	UINT8 NumInterfaces;
	UINT8 ConfigurationValue;
	UINT8 COnfiguration;
	UINT8 Attributes;
	UINT8 MaxPower;
} EFI_USB_CONFIG_DESCRIPTOR;

typedef struct {
	EFI_USB_CONFIG_DESCRIPTOR *ConfigDescriptor;
	EFI_USB_INTERFACE_INFO **InterfaceInfoTable;
} EFI_USB_CONFIG_INFO;

typedef struct {
	UINT8 Length;
	UINT8 DescriptorType;
	UINT16 BcdUSB;
	UINT8 DeviceClass;
	UINT8 DeviceSubClass;
	UINT8 DeviceProtocol;
	UINT8 MaxPacketSize0;
	UINT16 IdVendor;
	UINT16 IdProduct;
	UINT16 BcdDevice;
	UINT8 StrManufacturer;
	UINT8 StrProduct;
	UINT8 StrSerialNumber;
	UINT8 NumConfigurations;
} EFI_USB_DEVICE_DESCRIPTOR;

typedef struct {
	EFI_USB_DEVICE_DESCRIPTOR *DeviceDescriptor;
	EFI_USB_CONFIG_INFO **ConfigInfoTable;
} EFI_USB_DEVICE_INFO;

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_CONFIGURE_ENABLE_ENDPOINTS) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN EFI_USB_DEVICE_INFO *DeviceInfo
	);

typedef enum _EFI_USB_ENDPOINT_TYPE {
	UsbEndpointControl = 0x00,
	//UsbEndpointIsochronous = 0x01,
	UsbEndpointBulk = 0x02,
	//UsbEndpointInterrupt = 0x03
} EFI_USB_ENDPOINT_TYPE;

typedef enum _EFI_USB_BUS_SPEED {
	UsbBusSpeedUnknown = 0,
	UsbBusSpeedLow,
	UsbBusSpeedFull,
	UsbBusSpeedHigh,
	UsbBusSpeedSuper,
	UsbBusSpeedMaximum = UsbBusSpeedSuper
} EFI_USB_BUS_SPEED;

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_GET_ENDPOINT_MAXPACKET_SIZE) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN EFI_USB_ENDPOINT_TYPE EndpointType,
	IN EFI_USB_BUS_SPEED BusSpeed,
	OUT UINT16 *MaxPacketSize
	);

typedef enum _EFI_USBFN_DEVICE_INFO_ID {
	EfiUsbDeviceInfoUnknown = 0,
	EfiUsbDeviceInfoSerialNumber,
	EfiUsbDeviceInfoManufacturerName,
	EfiUsbDeviceInfoProductName
} EFI_USBFN_DEVICE_INFO_ID;

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_GET_DEVICE_INFO) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN EFI_USBFN_DEVICE_INFO_ID Id,
	IN OUT UINTN *BufferSize,
	OUT VOID *Buffer OPTIONAL
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_GET_VENDOR_ID_PRODUCT_ID) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	OUT UINT16 *Vid,
	OUT UINT16 *Pid
	);

typedef enum _EFI_USBFN_ENDPOINT_DIRECTION {
	EfiUsbEndpointDirectionHostOut = 0,
	EfiUsbEndpointDirectionHostIn,
	EfiUsbEndpointDirectionDeviceTx = EfiUsbEndpointDirectionHostIn,
	EfiUsbEndpointDirectionDeviceRx = EfiUsbEndpointDirectionHostOut
} EFI_USBFN_ENDPOINT_DIRECTION;

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_ABORT_TRANSFER) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN UINT8 EndpointIndex,
	IN EFI_USBFN_ENDPOINT_DIRECTION Direction;
	);
	
typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_GET_ENDPOINT_STALL_STATE) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN UINT8 EndpointIndx,
	IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
	IN OUT BOOLEAN *State
	);
	
typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_SET_ENDPOINT_STALL_STATE) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN UINT8 EndpointIndex,
	IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
	IN BOOLEAN State
	);

typedef enum _EFI_USBFN_MESSAGE {
	EfiUsbMsgNone = 0,
	EfiUsbMsgSetupPacket, /*SETUP packet received, returned buffer contains EFI_USB_DEVICE_REQUEST struct*/
	EfiUsbMsgEndpointStatusChangedRx, /*Some of data has been transmitted to host; responsibility of driver if data needs to e resent; returned buffer contains EFI_USBFN_TRANSFER_RESULT struct*/
	EfiUsbMsgBusEventDetach,
	EfiUsbMsgBusEventAttach,
	EfiUsbMsgBusEventReset,
	EfiUsbMsgBusEventSuspend,
	EfiUsbMsgBusEventResume,
	EfiUsbMsgBusEventSpeed /*Bus Speed updated, returned buffer using EFI_USB_BUS_SPEED enumeration*/
} EFI_USBFN_MESSAGE;

typedef enum _EFI_USBFN_TRANSFER_STATUS {
	UsbTransferStatusUnknown = 0,
	UsbTransferStatusComplete,
	UsbTransferStatusAborted,
	UsbTransferStatusActive,
	UsbTransferStatusNone
} EFI_USBFN_TRANSFER_STATUS;

typedef struct _EFI_USBFN_TRANSFER_RESULT {
	UINTN BytesTransferred;
	EFI_USBFN_TRANSFER_STATUS TransferStatus;
	UINT8 EndpointIndex;
	EFI_USBFN_ENDPOINT_DIRECTION Direction;
	VOID *Buffer;
} EFI_USBFN_TRANSFER_RESULT;

typedef struct {
	UINT8 RequestType;
	UINT8 Request;
	UINT16 Value;
	UINT16 Index;
	UINT16 Length;
} EFI_USB_DEVICE_REQUEST;

typedef union _EFI_USBFN_MESSAGE_PAYLOAD {
	EFI_USB_DEVICE_REQUEST udr;
	EFI_USBFN_TRANSFER_RESULT utr;
	EFI_USB_BUS_SPEED ubs;
} EFI_USBFN_MESSAGE_PAYLOAD;

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_EVENTHANDLER) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	OUT EFI_USBFN_MESSAGE *Message,
	IN OUT UINTN *PayloadSize,
	OUT EFI_USBFN_MESSAGE_PAYLOAD *Payload
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_TRANSFER) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN UINT8 EndpointIndex,
	IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
	IN OUT UINTN *BufferSize,
	IN OUT VOID *Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_GET_MAXTRANSFER_SIZE) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	OUT UINTN *MaxTransferSize
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_ALLOCATE_TRANSFER_BUFFER) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN UINTN Size,
	OUT VOID **Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_FREE_TRANSFER_BUFFER) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN VOID *Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_START_CONTROLLER) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_IO_STOP_CONTROLLER) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This
	);

typedef enum _EFI_USBFN_POLICY_TYPE {
	EfiUsbPolicyUndefined = 0,
	EfiUsbPolicyMaxTransactionSize,
	EfiUsbPolicyZeroLengthTerminationSupport,
	EfiUsbPolicyZeroLengthTermination
} EFI_USBFN_POLICY_TYPE;

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_SET_ENDPOINT_POLICY) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN UINT8 EndpointIndex,
	IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
	IN EFI_USBFN_POLICY_TYPE PolicyType,
	IN UINTN BufferSize,
	IN VOID *Buffer
	);

typedef
EFI_STATUS
(EFIAPI* EFI_USBFN_GET_ENDPOINT_POLICY) (
	IN struct _EFI_USBFN_IO_PROTOCOL *This,
	IN UINT8 EndpointIndex,
	IN EFI_USBFN_ENDPOINT_DIRECTION Direction,
	IN EFI_USBFN_POLICY_TYPE PolicyType,
	IN OUT UINTN *BufferSize,
	IN OUT VOID *Buffer
	);

static EFI_GUID EFI_USBFN_IO_PROTOCOL_GUID = {.Data1 = 0x32d963a, .Data2 = 0xfe5d, .Data3 = 0x4f30, .Data4 = {0xb6, 0x33, 0x6e, 0x5d, 0xc5, 0x58, 0x3, 0xcc}};

typedef struct _EFI_USBFN_IO_PROTOCOL {
	UINT32 Revision;
	EFI_USBFN_IO_DETECT_PORT DetectPort;
	EFI_USBFN_IO_CONFIGURE_ENABLE_ENDPOINTS ConfigureEnableEndpoints;
	
	EFI_USBFN_IO_GET_ENDPOINT_MAXPACKET_SIZE GetEndpointMaxPacketSize;
	
	EFI_USBFN_IO_GET_DEVICE_INFO GetDeviceInfo;
	EFI_USBFN_IO_GET_VENDOR_ID_PRODUCT_ID GetVendorIdProductId;
	
	EFI_USBFN_IO_ABORT_TRANSFER AbortTransfer;
	EFI_USBFN_IO_GET_ENDPOINT_STALL_STATE GetEndpointStallState;
	
	EFI_USBFN_IO_SET_ENDPOINT_STALL_STATE SetEndpointStallState;
	
	EFI_USBFN_IO_EVENTHANDLER EventHandler;
	EFI_USBFN_IO_TRANSFER Transfer;
	EFI_USBFN_IO_GET_MAXTRANSFER_SIZE GetMaxTransferSize;
	
	EFI_USBFN_IO_ALLOCATE_TRANSFER_BUFFER AllocateTransferBuffer;
	EFI_USBFN_IO_FREE_TRANSFER_BUFFER FreeTransferBuffer;
	
	EFI_USBFN_IO_START_CONTROLLER StartController;
	EFI_USBFN_IO_STOP_CONTROLLER StopController;
	
	EFI_USBFN_SET_ENDPOINT_POLICY SetEndpointPolicy;
	EFI_USBFN_GET_ENDPOINT_POLICY GetEndpointPolicy;
} EFI_USBFN_IO_PROTOCOL;



static EFI_GUID EFI_USB2_HC_PROTOCOL_GUID = {.Data1 = 0x3e745226, .Data2 = 0x9818, .Data3 = 0x45b6, .Data4 = {0xa2, 0xac, 0xd7, 0xcd, 0x0e, 0x8b, 0xa2, 0xbc}};

#define EFI_USB_SPEED_FULL 0x0000 
#define EFI_USB_SPEED_LOW 0x0001 
#define EFI_USB_SPEED_HIGH 0x0002 
#define EFI_USB_SPEED_SUPER 0x0003

typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_GET_CAPABILITY) (
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	OUT UINT8 *MaxSpeed, 
	OUT UINT8 *PortNumber, 
	OUT UINT8 *Is64BitCapable 
	);

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_RESET) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT16 Attributes 
);

typedef enum { 
	EfiUsbHcStateHalt, 
	EfiUsbHcStateOperational, 
	EfiUsbHcStateSuspend, 
	EfiUsbHcStateMaximum 
} EFI_USB_HC_STATE;

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_GET_STATE) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	OUT EFI_USB_HC_STATE *State 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_SET_STATE) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN EFI_USB_HC_STATE State 
);

typedef struct { 
	UINT8 TranslatorHubAddress;
	UINT8 TranslatorPortNumber;
} EFI_USB2_HC_TRANSACTION_TRANSLATOR;

typedef enum {
	EfiUsbDataIn,
	EfiUsbDataOut,
	EfiUsbNoData
} EFI_USB_DATA_DIRECTION;

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 DeviceAddress, 
	IN UINT8 DeviceSpeed, 
	IN UINTN MaximumPacketLength, 
	IN EFI_USB_DEVICE_REQUEST *Request, 
	IN EFI_USB_DATA_DIRECTION TransferDirection, 
	IN OUT VOID *Data OPTIONAL, 
	IN OUT UINTN *DataLength OPTIONAL, 
	IN UINTN TimeOut, 
	IN EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, 
	OUT UINT32 *TransferResult 
);

#define EFI_USB_MAX_BULK_BUFFER_NUM 10

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_BULK_TRANSFER) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 DeviceAddress, 
	IN UINT8 EndPointAddress, 
	IN UINT8 DeviceSpeed, 
	IN UINTN MaximumPacketLength, 
	IN UINT8 DataBuffersNumber, 
	IN OUT VOID *Data[EFI_USB_MAX_BULK_BUFFER_NUM], 
	IN OUT UINTN *DataLength, 
	IN OUT UINT8 *DataToggle, 
	IN UINTN TimeOut, 
	IN EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, 
	OUT UINT32 *TransferResult 
);

typedef 
EFI_STATUS 
(EFIAPI * EFI_ASYNC_USB_TRANSFER_CALLBACK) ( 
	IN VOID *Data, 
	IN UINTN DataLength, 
	IN VOID *Context, 
	IN UINT32 Status 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 DeviceAddress, 
	IN UINT8 EndPointAddress, 
	IN UINT8 DeviceSpeed, 
	IN UINTN MaximumPacketLength, 
	IN BOOLEAN IsNewTransfer, 
	IN OUT UINT8 *DataToggle, 
	IN UINTN PollingInterval OPTIONAL, 
	IN UINTN DataLength OPTIONAL, 
	IN EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator OPTIONAL, 
	IN EFI_ASYNC_USB_TRANSFER_CALLBACK CallBackFunction OPTIONAL, 
	IN VOID *Context OPTIONAL 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 DeviceAddress, 
	IN UINT8 EndPointAddress, 
	IN UINT8 DeviceSpeed, 
	IN UINTN MaximumPacketLength, 
	IN OUT VOID *Data, 
	IN OUT UINTN *DataLength, 
	IN OUT UINT8 *DataToggle, 
	IN UINTN TimeOut, 
	IN EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
	OUT UINT32 *TransferResult 
);

#define EFI_USB_MAX_ISO_BUFFER_NUM 7
#define EFI_USB_MAX_ISO_BUFFER_NUM1 2

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 DeviceAddress, 
	IN UINT8 EndPointAddress, 
	IN UINT8 DeviceSpeed, 
	IN UINTN MaximumPacketLength, 
	IN UINT8 DataBuffersNumber, 
	IN OUT VOID *Data[EFI_USB_MAX_ISO_BUFFER_NUM], 
	IN UINTN DataLength, 
	IN EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, 
	OUT UINT32 *TransferResult 
);

typedef 
EFI_STATUS 
(EFIAPI * EFI_USB2_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 DeviceAddress, 
	IN UINT8 EndPointAddress, 
	IN UINT8 DeviceSpeed, 
	IN UINTN MaximumPacketLength, 
	IN UINT8 DataBuffersNumber, 
	IN OUT VOID *Data[EFI_USB_MAX_ISO_BUFFER_NUM], 
	IN UINTN DataLength, 
	IN EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator, 
	IN EFI_ASYNC_USB_TRANSFER_CALLBACK IsochronousCallBack, 
	IN VOID *Context OPTIONAL 
);

typedef struct { 
	UINT16 PortStatus; 
	UINT16 PortChangeStatus; 
} EFI_USB_PORT_STATUS;

#define USB_PORT_STAT_CONNECTION 0x0001 
#define USB_PORT_STAT_ENABLE 0x0002 
#define USB_PORT_STAT_SUSPEND 0x0004 
#define USB_PORT_STAT_OVERCURRENT 0x0008 
#define USB_PORT_STAT_RESET 0x0010 
#define USB_PORT_STAT_POWER 0x0100 
#define USB_PORT_STAT_LOW_SPEED 0x0200 
#define USB_PORT_STAT_HIGH_SPEED 0x0400 
#define USB_PORT_STAT_SUPER_SPEED 0x0800 
#define USB_PORT_STAT_OWNER 0x2000

#define USB_PORT_STAT_C_CONNECTION 0x0001 
#define USB_PORT_STAT_C_ENABLE 0x0002 
#define USB_PORT_STAT_C_SUSPEND 0x0004 
#define USB_PORT_STAT_C_OVERCURRENT 0x0008 
#define USB_PORT_STAT_C_RESET 0x0010

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 PortNumber, 
	OUT EFI_USB_PORT_STATUS *PortStatus 
);

typedef enum { 
	EfiUsbPortEnable = 1, 
	EfiUsbPortSuspend = 2, 
	EfiUsbPortReset = 4, 
	EfiUsbPortPower = 8, 
	EfiUsbPortOwner = 13, 
	EfiUsbPortConnectChange = 16, 
	EfiUsbPortEnableChange = 17, 
	EfiUsbPortSuspendChange = 18, 
	EfiUsbPortOverCurrentChange = 19, 
	EfiUsbPortResetChange = 20 
} EFI_USB_PORT_FEATURE;

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 PortNumber, 
	IN EFI_USB_PORT_FEATURE PortFeature 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE) ( 
	IN struct _EFI_USB2_HC_PROTOCOL *This, 
	IN UINT8 PortNumber, 
	IN EFI_USB_PORT_FEATURE PortFeature 
);

typedef struct _EFI_USB2_HC_PROTOCOL { 
	EFI_USB2_HC_PROTOCOL_GET_CAPABILITY GetCapability; 
	EFI_USB2_HC_PROTOCOL_RESET Reset; 
	
	EFI_USB2_HC_PROTOCOL_GET_STATE GetState; 
	EFI_USB2_HC_PROTOCOL_SET_STATE SetState; 
	
	EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER ControlTransfer; 
	
	EFI_USB2_HC_PROTOCOL_BULK_TRANSFER BulkTransfer; 
	
	EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER AsyncInterruptTransfer; 
	EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER SyncInterruptTransfer; 
	
	EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER IsochronousTransfer; 
	EFI_USB2_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER AsyncIsochronousTransfer; 
	
	EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS GetRootHubPortStatus; 
	EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE SetRootHubPortFeature; 
	EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE ClearRootHubPortFeature;
	
	UINT16 MajorRevision; 
	UINT16 MinorRevision; 
} EFI_USB2_HC_PROTOCOL;

static EFI_GUID EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID = {.Data1 = 0x964e5b22, .Data2 = 0x6459, .Data3 = 0x11d2, .Data4 = {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

typedef 
EFI_STATUS 
(EFIAPI *EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME) ( 
	IN struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, 
	OUT struct _EFI_FILE_PROTOCOL **Root 
);

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL { 
	UINT64 Revision; 
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume; 
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

//****************************************************** // Open Modes //****************************************************** 
#define EFI_FILE_MODE_READ 0x0000000000000001 
#define EFI_FILE_MODE_WRITE 0x0000000000000002 
#define EFI_FILE_MODE_CREATE 0x8000000000000000 
//****************************************************** // File Attributes //****************************************************** 
#define EFI_FILE_READ_ONLY 0x0000000000000001 
#define EFI_FILE_HIDDEN 0x0000000000000002 
#define EFI_FILE_SYSTEM 0x0000000000000004 
#define EFI_FILE_RESERVED 0x0000000000000008 
#define EFI_FILE_DIRECTORY 0x0000000000000010 
#define EFI_FILE_ARCHIVE 0x0000000000000020 
#define EFI_FILE_VALID_ATTR

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_OPEN) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	OUT struct _EFI_FILE_PROTOCOL **NewHandle, 
	IN CHAR16 *FileName, 
	IN UINT64 OpenMode, 
	IN UINT64 Attributes 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_CLOSE) ( 
	IN struct _EFI_FILE_PROTOCOL *This 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_DELETE) ( 
	IN struct _EFI_FILE_PROTOCOL *This 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_READ) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN OUT UINTN *BufferSize, 
	OUT VOID *Buffer 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_WRITE) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN OUT UINTN *BufferSize, 
	IN VOID *Buffer 
);

typedef struct { 
	EFI_EVENT Event; 
	EFI_STATUS Status; 
	UINTN BufferSize; 
	VOID *Buffer; 
} EFI_FILE_IO_TOKEN;

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_OPEN_EX) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	OUT struct _EFI_FILE_PROTOCOL **NewHandle, 
	IN CHAR16 *FileName, 
	IN UINT64 OpenMode, 
	IN UINT64 Attributes, 
	IN OUT EFI_FILE_IO_TOKEN *Token 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_READ_EX) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN OUT EFI_FILE_IO_TOKEN *Token 
);

typedef
EFI_STATUS 
(EFIAPI *EFI_FILE_WRITE_EX) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN OUT EFI_FILE_IO_TOKEN *Token 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_FLUSH_EX) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN OUT EFI_FILE_IO_TOKEN *Token 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_SET_POSITION) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN UINT64 Position 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_GET_POSITION) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	OUT UINT64 *Position 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_GET_INFO) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN EFI_GUID *InformationType, 
	IN OUT UINTN *BufferSize, 
	OUT VOID *Buffer 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_SET_INFO) ( 
	IN struct _EFI_FILE_PROTOCOL *This, 
	IN EFI_GUID *InformationType, 
	IN UINTN BufferSize, 
	IN VOID *Buffer 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_FILE_FLUSH) ( 
	IN struct _EFI_FILE_PROTOCOL *This 
);

static EFI_GUID EFI_FILE_INFO_ID = {.Data1 = 0x09576e92, .Data2 = 0x6d3f, .Data3 = 0x11d2, .Data4 = {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

typedef struct { 
	UINT64 Size; 
	UINT64 FileSize; 
	UINT64 PhysicalSize; 
	EFI_TIME CreateTime; 
	EFI_TIME LastAccessTime; 
	EFI_TIME ModificationTime; 
	UINT64 Attribute; 
	CHAR16 FileName []; 
} EFI_FILE_INFO; 

//****************************************** // File Attribute Bits //****************************************** 
#define EFI_FILE_READ_ONLY 0x0000000000000001 
#define EFI_FILE_HIDDEN 0x0000000000000002 
#define EFI_FILE_SYSTEM 0x0000000000000004 
#define EFI_FILE_RESERVED 0x0000000000000008 
#define EFI_FILE_DIRECTORY 0x0000000000000010 
#define EFI_FILE_ARCHIVE 0x0000000000000020 

static EFI_GUID EFI_FILE_SYSTEM_INFO_ID = {.Data1 = 0x09576e93, .Data2 = 0x6d3f, .Data3 = 0x11d2, .Data4 = {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

typedef struct { 
	UINT64 Size; 
	BOOLEAN ReadOnly; 
	UINT64 VolumeSize; 
	UINT64 FreeSpace; 
	UINT32 BlockSize; 
	CHAR16 VolumeLabel[]; 
} EFI_FILE_SYSTEM_INFO;

static EFI_GUID EFI_FILE_SYSTEM_VOLUME_LABEL_ID = {.Data1 = 0xdb47d7d3, .Data2 = 0xfe81, .Data3 = 0x11d3, .Data4 = {0x9a, 0x35, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};

typedef struct { 
	CHAR16 VolumeLabel[]; 
} EFI_FILE_SYSTEM_VOLUME_LABEL;

typedef struct _EFI_FILE_PROTOCOL { 
	UINT64 Revision; 
	EFI_FILE_OPEN Open; 
	EFI_FILE_CLOSE Close; 
	EFI_FILE_DELETE Delete;
	
	EFI_FILE_READ Read; 
	EFI_FILE_WRITE Write;
	
	EFI_FILE_GET_POSITION GetPosition; 
	EFI_FILE_SET_POSITION SetPosition;
	
	EFI_FILE_GET_INFO GetInfo; 
	EFI_FILE_SET_INFO SetInfo;
	
	EFI_FILE_FLUSH Flush;
	
	EFI_FILE_OPEN_EX OpenEx; // Added for revision 2 
	EFI_FILE_READ_EX ReadEx;
	EFI_FILE_WRITE_EX WriteEx; // Added for revision 2 
	EFI_FILE_FLUSH_EX FlushEx; // Added for revision 2 
} EFI_FILE_PROTOCOL;

#endif