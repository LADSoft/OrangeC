#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /* COM_NO_WINDOWS_H */

#ifndef _WINCODEC_H
#define _WINCODEC_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */

#ifndef __IWICPalette_FWD_DEFINED__
#define __IWICPalette_FWD_DEFINED__
typedef interface IWICPalette IWICPalette;
#endif /* __IWICPalette_FWD_DEFINED__ */

#ifndef __IWICBitmapSource_FWD_DEFINED__
#define __IWICBitmapSource_FWD_DEFINED__
typedef interface IWICBitmapSource IWICBitmapSource;
#endif /* __IWICBitmapSource_FWD_DEFINED__ */

#ifndef __IWICFormatConverter_FWD_DEFINED__
#define __IWICFormatConverter_FWD_DEFINED__
typedef interface IWICFormatConverter IWICFormatConverter;
#endif /* __IWICFormatConverter_FWD_DEFINED__ */

#ifndef __IWICPlanarFormatConverter_FWD_DEFINED__
#define __IWICPlanarFormatConverter_FWD_DEFINED__
typedef interface IWICPlanarFormatConverter IWICPlanarFormatConverter;
#endif /* __IWICPlanarFormatConverter_FWD_DEFINED__ */

#ifndef __IWICBitmapScaler_FWD_DEFINED__
#define __IWICBitmapScaler_FWD_DEFINED__
typedef interface IWICBitmapScaler IWICBitmapScaler;
#endif /* __IWICBitmapScaler_FWD_DEFINED__ */

#ifndef __IWICBitmapClipper_FWD_DEFINED__
#define __IWICBitmapClipper_FWD_DEFINED__
typedef interface IWICBitmapClipper IWICBitmapClipper;
#endif /* __IWICBitmapClipper_FWD_DEFINED__ */

#ifndef __IWICBitmapFlipRotator_FWD_DEFINED__
#define __IWICBitmapFlipRotator_FWD_DEFINED__
typedef interface IWICBitmapFlipRotator IWICBitmapFlipRotator;
#endif /* __IWICBitmapFlipRotator_FWD_DEFINED__ */

#ifndef __IWICBitmapLock_FWD_DEFINED__
#define __IWICBitmapLock_FWD_DEFINED__
typedef interface IWICBitmapLock IWICBitmapLock;
#endif /* __IWICBitmapLock_FWD_DEFINED__ */

#ifndef __IWICBitmap_FWD_DEFINED__
#define __IWICBitmap_FWD_DEFINED__
typedef interface IWICBitmap IWICBitmap;
#endif /* __IWICBitmap_FWD_DEFINED__ */

#ifndef __IWICColorContext_FWD_DEFINED__
#define __IWICColorContext_FWD_DEFINED__
typedef interface IWICColorContext IWICColorContext;
#endif /* __IWICColorContext_FWD_DEFINED__ */

#ifndef __IWICColorTransform_FWD_DEFINED__
#define __IWICColorTransform_FWD_DEFINED__
typedef interface IWICColorTransform IWICColorTransform;
#endif /* __IWICColorTransform_FWD_DEFINED__ */

#ifndef __IWICFastMetadataEncoder_FWD_DEFINED__
#define __IWICFastMetadataEncoder_FWD_DEFINED__
typedef interface IWICFastMetadataEncoder IWICFastMetadataEncoder;
#endif /* __IWICFastMetadataEncoder_FWD_DEFINED__ */

#ifndef __IWICStream_FWD_DEFINED__
#define __IWICStream_FWD_DEFINED__
typedef interface IWICStream IWICStream;
#endif /* __IWICStream_FWD_DEFINED__ */

#ifndef __IWICEnumMetadataItem_FWD_DEFINED__
#define __IWICEnumMetadataItem_FWD_DEFINED__
typedef interface IWICEnumMetadataItem IWICEnumMetadataItem;
#endif /* __IWICEnumMetadataItem_FWD_DEFINED__ */

#ifndef __IWICMetadataQueryReader_FWD_DEFINED__
#define __IWICMetadataQueryReader_FWD_DEFINED__
typedef interface IWICMetadataQueryReader IWICMetadataQueryReader;
#endif /* __IWICMetadataQueryReader_FWD_DEFINED__ */

#ifndef __IWICMetadataQueryWriter_FWD_DEFINED__
#define __IWICMetadataQueryWriter_FWD_DEFINED__
typedef interface IWICMetadataQueryWriter IWICMetadataQueryWriter;
#endif /* __IWICMetadataQueryWriter_FWD_DEFINED__ */

#ifndef __IWICBitmapEncoder_FWD_DEFINED__
#define __IWICBitmapEncoder_FWD_DEFINED__
typedef interface IWICBitmapEncoder IWICBitmapEncoder;
#endif /* __IWICBitmapEncoder_FWD_DEFINED__ */

#ifndef __IWICBitmapFrameEncode_FWD_DEFINED__
#define __IWICBitmapFrameEncode_FWD_DEFINED__
typedef interface IWICBitmapFrameEncode IWICBitmapFrameEncode;
#endif /* __IWICBitmapFrameEncode_FWD_DEFINED__ */

#ifndef __IWICPlanarBitmapFrameEncode_FWD_DEFINED__
#define __IWICPlanarBitmapFrameEncode_FWD_DEFINED__
typedef interface IWICPlanarBitmapFrameEncode IWICPlanarBitmapFrameEncode;
#endif /* __IWICPlanarBitmapFrameEncode_FWD_DEFINED__ */

#ifndef __IWICImageEncoder_FWD_DEFINED__
#define __IWICImageEncoder_FWD_DEFINED__
typedef interface IWICImageEncoder IWICImageEncoder;
#endif /* __IWICImageEncoder_FWD_DEFINED__ */

#ifndef __IWICBitmapDecoder_FWD_DEFINED__
#define __IWICBitmapDecoder_FWD_DEFINED__
typedef interface IWICBitmapDecoder IWICBitmapDecoder;
#endif /* __IWICBitmapDecoder_FWD_DEFINED__ */

#ifndef __IWICBitmapSourceTransform_FWD_DEFINED__
#define __IWICBitmapSourceTransform_FWD_DEFINED__
typedef interface IWICBitmapSourceTransform IWICBitmapSourceTransform;
#endif /* __IWICBitmapSourceTransform_FWD_DEFINED__ */

#ifndef __IWICPlanarBitmapSourceTransform_FWD_DEFINED__
#define __IWICPlanarBitmapSourceTransform_FWD_DEFINED__
typedef interface IWICPlanarBitmapSourceTransform IWICPlanarBitmapSourceTransform;
#endif /* __IWICPlanarBitmapSourceTransform_FWD_DEFINED__ */

#ifndef __IWICBitmapFrameDecode_FWD_DEFINED__
#define __IWICBitmapFrameDecode_FWD_DEFINED__
typedef interface IWICBitmapFrameDecode IWICBitmapFrameDecode;
#endif /* __IWICBitmapFrameDecode_FWD_DEFINED__ */

#ifndef __IWICProgressiveLevelControl_FWD_DEFINED__
#define __IWICProgressiveLevelControl_FWD_DEFINED__
typedef interface IWICProgressiveLevelControl IWICProgressiveLevelControl;
#endif /* __IWICProgressiveLevelControl_FWD_DEFINED__ */

#ifndef __IWICProgressCallback_FWD_DEFINED__
#define __IWICProgressCallback_FWD_DEFINED__
typedef interface IWICProgressCallback IWICProgressCallback;
#endif /* __IWICProgressCallback_FWD_DEFINED__ */

#ifndef __IWICBitmapCodecProgressNotification_FWD_DEFINED__
#define __IWICBitmapCodecProgressNotification_FWD_DEFINED__
typedef interface IWICBitmapCodecProgressNotification IWICBitmapCodecProgressNotification;
#endif /* __IWICBitmapCodecProgressNotification_FWD_DEFINED__ */

#ifndef __IWICComponentInfo_FWD_DEFINED__
#define __IWICComponentInfo_FWD_DEFINED__
typedef interface IWICComponentInfo IWICComponentInfo;
#endif /* __IWICComponentInfo_FWD_DEFINED__ */

#ifndef __IWICFormatConverterInfo_FWD_DEFINED__
#define __IWICFormatConverterInfo_FWD_DEFINED__
typedef interface IWICFormatConverterInfo IWICFormatConverterInfo;
#endif /* __IWICFormatConverterInfo_FWD_DEFINED__ */

#ifndef __IWICBitmapCodecInfo_FWD_DEFINED__
#define __IWICBitmapCodecInfo_FWD_DEFINED__
typedef interface IWICBitmapCodecInfo IWICBitmapCodecInfo;
#endif /* __IWICBitmapCodecInfo_FWD_DEFINED__ */

#ifndef __IWICBitmapEncoderInfo_FWD_DEFINED__
#define __IWICBitmapEncoderInfo_FWD_DEFINED__
typedef interface IWICBitmapEncoderInfo IWICBitmapEncoderInfo;
#endif /* __IWICBitmapEncoderInfo_FWD_DEFINED__ */

#ifndef __IWICBitmapDecoderInfo_FWD_DEFINED__
#define __IWICBitmapDecoderInfo_FWD_DEFINED__
typedef interface IWICBitmapDecoderInfo IWICBitmapDecoderInfo;
#endif /* __IWICBitmapDecoderInfo_FWD_DEFINED__ */

#ifndef __IWICPixelFormatInfo_FWD_DEFINED__
#define __IWICPixelFormatInfo_FWD_DEFINED__
typedef interface IWICPixelFormatInfo IWICPixelFormatInfo;
#endif /* __IWICPixelFormatInfo_FWD_DEFINED__ */

#ifndef __IWICPixelFormatInfo2_FWD_DEFINED__
#define __IWICPixelFormatInfo2_FWD_DEFINED__
typedef interface IWICPixelFormatInfo2 IWICPixelFormatInfo2;
#endif /* __IWICPixelFormatInfo2_FWD_DEFINED__ */

#ifndef __IWICImagingFactory_FWD_DEFINED__
#define __IWICImagingFactory_FWD_DEFINED__
typedef interface IWICImagingFactory IWICImagingFactory;
#endif /* __IWICImagingFactory_FWD_DEFINED__ */

#ifndef __IWICImagingFactory2_FWD_DEFINED__
#define __IWICImagingFactory2_FWD_DEFINED__
typedef interface IWICImagingFactory2 IWICImagingFactory2;
#endif /* __IWICImagingFactory2_FWD_DEFINED__ */

#ifndef __IWICDevelopRawNotificationCallback_FWD_DEFINED__
#define __IWICDevelopRawNotificationCallback_FWD_DEFINED__
typedef interface IWICDevelopRawNotificationCallback IWICDevelopRawNotificationCallback;
#endif /* __IWICDevelopRawNotificationCallback_FWD_DEFINED__ */

#ifndef __IWICDevelopRaw_FWD_DEFINED__
#define __IWICDevelopRaw_FWD_DEFINED__
typedef interface IWICDevelopRaw IWICDevelopRaw;
#endif /* __IWICDevelopRaw_FWD_DEFINED__ */

#ifndef __IWICDdsDecoder_FWD_DEFINED__
#define __IWICDdsDecoder_FWD_DEFINED__
typedef interface IWICDdsDecoder IWICDdsDecoder;
#endif /* __IWICDdsDecoder_FWD_DEFINED__ */

#ifndef __IWICDdsEncoder_FWD_DEFINED__
#define __IWICDdsEncoder_FWD_DEFINED__
typedef interface IWICDdsEncoder IWICDdsEncoder;
#endif /* __IWICDdsEncoder_FWD_DEFINED__ */

#ifndef __IWICDdsFrameDecode_FWD_DEFINED__
#define __IWICDdsFrameDecode_FWD_DEFINED__
typedef interface IWICDdsFrameDecode IWICDdsFrameDecode;
#endif /* __IWICDdsFrameDecode_FWD_DEFINED__ */

#include "wtypes.h"
#include "propidl.h"
#include "ocidl.h"
#include "dxgiformat.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* #include <intsafe.h> */

#define WINCODEC_SDK_VERSION1  0x0236
#define WINCODEC_SDK_VERSION2  0x0237
DEFINE_GUID(CLSID_WICImagingFactory,0xcacaf262,0x9370,0x4615,0xa1,0x3b,0x9f,0x55,0x39,0xda,0x4c,0xa);
DEFINE_GUID(CLSID_WICImagingFactory1,0xcacaf262,0x9370,0x4615,0xa1,0x3b,0x9f,0x55,0x39,0xda,0x4c,0xa);
DEFINE_GUID(CLSID_WICImagingFactory2,0x317d06e8,0x5f24,0x433d,0xbd,0xf7,0x79,0xce,0x68,0xd8,0xab,0xc2);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
#define WINCODEC_SDK_VERSION  WINCODEC_SDK_VERSION2
#define CLSID_WICImagingFactory  CLSID_WICImagingFactory2
#else /* _WIN32_WINNT < _WIN32_WINNT_WIN8 */
#define WINCODEC_SDK_VERSION  WINCODEC_SDK_VERSION1
#endif /* _WIN32_WINNT < _WIN32_WINNT_WIN8 */
DEFINE_GUID(GUID_VendorMicrosoft,0xf0e749ca,0xedef,0x4589,0xa7,0x3a,0xee,0xe,0x62,0x6a,0x2a,0x2b);
DEFINE_GUID(GUID_VendorMicrosoftBuiltIn,0x257a30fd,0x6b6,0x462b,0xae,0xa4,0x63,0xf7,0xb,0x86,0xe5,0x33);
DEFINE_GUID(CLSID_WICPngDecoder,0x389ea17b,0x5078,0x4cde,0xb6,0xef,0x25,0xc1,0x51,0x75,0xc7,0x51);
DEFINE_GUID(CLSID_WICPngDecoder1,0x389ea17b,0x5078,0x4cde,0xb6,0xef,0x25,0xc1,0x51,0x75,0xc7,0x51);
DEFINE_GUID(CLSID_WICPngDecoder2,0xe018945b,0xaa86,0x4008,0x9b,0xd4,0x67,0x77,0xa1,0xe4,0x0c,0x11);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
#define CLSID_WICPngDecoder  CLSID_WICPngDecoder2
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */
DEFINE_GUID(CLSID_WICBmpDecoder,0x6b462062,0x7cbf,0x400d,0x9f,0xdb,0x81,0x3d,0xd1,0x0f,0x27,0x78);
DEFINE_GUID(CLSID_WICIcoDecoder,0xc61bfcdf,0x2e0f,0x4aad,0xa8,0xd7,0xe0,0x6b,0xaf,0xeb,0xcd,0xfe);
DEFINE_GUID(CLSID_WICJpegDecoder,0x9456a480,0xe88b,0x43ea,0x9e,0x73,0x0b,0x2d,0x9b,0x71,0xb1,0xca);
DEFINE_GUID(CLSID_WICGifDecoder,0x381dda3c,0x9ce9,0x4834,0xa2,0x3e,0x1f,0x98,0xf8,0xfc,0x52,0xbe);
DEFINE_GUID(CLSID_WICTiffDecoder,0xb54e85d9,0xfe23,0x499f,0x8b,0x88,0x6a,0xce,0xa7,0x13,0x75,0x2b);
DEFINE_GUID(CLSID_WICWmpDecoder,0xa26cec36,0x234c,0x4950,0xae,0x16,0xe3,0x4a,0xac,0xe7,0x1d,0x0d);
DEFINE_GUID(CLSID_WICDdsDecoder,0x9053699f,0xa341,0x429d,0x9e,0x90,0xee,0x43,0x7c,0xf8,0x0c,0x73);
DEFINE_GUID(CLSID_WICBmpEncoder,0x69be8bb4,0xd66d,0x47c8,0x86,0x5a,0xed,0x15,0x89,0x43,0x37,0x82);
DEFINE_GUID(CLSID_WICPngEncoder,0x27949969,0x876a,0x41d7,0x94,0x47,0x56,0x8f,0x6a,0x35,0xa4,0xdc);
DEFINE_GUID(CLSID_WICJpegEncoder,0x1a34f5c1,0x4a5a,0x46dc,0xb6,0x44,0x1f,0x45,0x67,0xe7,0xa6,0x76);
DEFINE_GUID(CLSID_WICGifEncoder,0x114f5598,0x0b22,0x40a0,0x86,0xa1,0xc8,0x3e,0xa4,0x95,0xad,0xbd);
DEFINE_GUID(CLSID_WICTiffEncoder,0x0131be10,0x2001,0x4c5f,0xa9,0xb0,0xcc,0x88,0xfa,0xb6,0x4c,0xe8);
DEFINE_GUID(CLSID_WICWmpEncoder,0xac4ce3cb,0xe1c1,0x44cd,0x82,0x15,0x5a,0x16,0x65,0x50,0x9e,0xc2);
DEFINE_GUID(CLSID_WICDdsEncoder,0xa61dde94,0x66ce,0x4ac1,0x88,0x1b,0x71,0x68,0x05,0x88,0x89,0x5e);
DEFINE_GUID(GUID_ContainerFormatBmp,0x0af1d87e,0xfcfe,0x4188,0xbd,0xeb,0xa7,0x90,0x64,0x71,0xcb,0xe3);
DEFINE_GUID(GUID_ContainerFormatPng,0x1b7cfaf4,0x713f,0x473c,0xbb,0xcd,0x61,0x37,0x42,0x5f,0xae,0xaf);
DEFINE_GUID(GUID_ContainerFormatIco,0xa3a860c4,0x338f,0x4c17,0x91,0x9a,0xfb,0xa4,0xb5,0x62,0x8f,0x21);
DEFINE_GUID(GUID_ContainerFormatJpeg,0x19e4a5aa,0x5662,0x4fc5,0xa0,0xc0,0x17,0x58,0x02,0x8e,0x10,0x57);
DEFINE_GUID(GUID_ContainerFormatTiff,0x163bcc30,0xe2e9,0x4f0b,0x96,0x1d,0xa3,0xe9,0xfd,0xb7,0x88,0xa3);
DEFINE_GUID(GUID_ContainerFormatGif,0x1f8a5601,0x7d4d,0x4cbd,0x9c,0x82,0x1b,0xc8,0xd4,0xee,0xb9,0xa5);
DEFINE_GUID(GUID_ContainerFormatWmp,0x57a37caa,0x367a,0x4540,0x91,0x6b,0xf1,0x83,0xc5,0x09,0x3a,0x4b);
DEFINE_GUID(GUID_ContainerFormatDds,0x9967cb95,0x2e85,0x4ac8,0x8c,0xa2,0x83,0xd7,0xcc,0xd4,0x25,0xc9);
DEFINE_GUID(CLSID_WICImagingCategories,0xfae3d380,0xfea4,0x4623,0x8c,0x75,0xc6,0xb6,0x11,0x10,0xb6,0x81);
DEFINE_GUID(CATID_WICBitmapDecoders,0x7ed96837,0x96f0,0x4812,0xb2,0x11,0xf1,0x3c,0x24,0x11,0x7e,0xd3);
DEFINE_GUID(CATID_WICBitmapEncoders,0xac757296,0x3522,0x4e11,0x98,0x62,0xc1,0x7b,0xe5,0xa1,0x76,0x7e);
DEFINE_GUID(CATID_WICPixelFormats,0x2b46e70f,0xcda7,0x473e,0x89,0xf6,0xdc,0x96,0x30,0xa2,0x39,0x0b);
DEFINE_GUID(CATID_WICFormatConverters,0x7835eae8,0xbf14,0x49d1,0x93,0xce,0x53,0x3a,0x40,0x7b,0x22,0x48);
DEFINE_GUID(CATID_WICMetadataReader,0x05af94d8,0x7174,0x4cd2,0xbe,0x4a,0x41,0x24,0xb8,0x0e,0xe4,0xb8);
DEFINE_GUID(CATID_WICMetadataWriter,0xabe3b9a4,0x257d,0x4b97,0xbd,0x1a,0x29,0x4a,0xf4,0x96,0x22,0x2e);
DEFINE_GUID(CLSID_WICDefaultFormatConverter,0x1a3f11dc,0xb514,0x4b17,0x8c,0x5f,0x21,0x54,0x51,0x38,0x52,0xf1);
DEFINE_GUID(CLSID_WICFormatConverterHighColor,0xac75d454,0x9f37,0x48f8,0xb9,0x72,0x4e,0x19,0xbc,0x85,0x60,0x11);
DEFINE_GUID(CLSID_WICFormatConverterNChannel,0xc17cabb2,0xd4a3,0x47d7,0xa5,0x57,0x33,0x9b,0x2e,0xfb,0xd4,0xf1);
DEFINE_GUID(CLSID_WICFormatConverterWMPhoto,0x9cb5172b,0xd600,0x46ba,0xab,0x77,0x77,0xbb,0x7e,0x3a,0x00,0xd9);
DEFINE_GUID(CLSID_WICPlanarFormatConverter,0x184132b8,0x32f8,0x4784,0x91,0x31,0xdd,0x72,0x24,0xb2,0x34,0x38);

typedef interface ID2D1Device ID2D1Device;
typedef interface ID2D1Image ID2D1Image;
#if 1
typedef DWORD *D2D1_PIXEL_FORMAT;
#endif

typedef UINT32 WICColor;

typedef struct WICRect {
    INT X;
    INT Y;
    INT Width;
    INT Height;
} WICRect;

typedef enum WICColorContextType {
    WICColorContextUninitialized = 0,
    WICColorContextProfile = 0x1,
    WICColorContextExifColorSpace = 0x2
} WICColorContextType;

typedef REFGUID REFWICPixelFormatGUID;

typedef GUID WICPixelFormatGUID;

#define GUID_WICPixelFormatUndefined  GUID_WICPixelFormatDontCare
DEFINE_GUID(GUID_WICPixelFormatDontCare,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x00);
DEFINE_GUID(GUID_WICPixelFormat1bppIndexed,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x01);
DEFINE_GUID(GUID_WICPixelFormat2bppIndexed,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x02);
DEFINE_GUID(GUID_WICPixelFormat4bppIndexed,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x03);
DEFINE_GUID(GUID_WICPixelFormat8bppIndexed,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x04);
DEFINE_GUID(GUID_WICPixelFormatBlackWhite,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x05);
DEFINE_GUID(GUID_WICPixelFormat2bppGray,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x06);
DEFINE_GUID(GUID_WICPixelFormat4bppGray,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x07);
DEFINE_GUID(GUID_WICPixelFormat8bppGray,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x08);
DEFINE_GUID(GUID_WICPixelFormat8bppAlpha,0xe6cd0116,0xeeba,0x4161,0xaa,0x85,0x27,0xdd,0x9f,0xb3,0xa8,0x95);
DEFINE_GUID(GUID_WICPixelFormat16bppBGR555,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x09);
DEFINE_GUID(GUID_WICPixelFormat16bppBGR565,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x0a);
DEFINE_GUID(GUID_WICPixelFormat16bppBGRA5551,0x05ec7c2b,0xf1e6,0x4961,0xad,0x46,0xe1,0xcc,0x81,0x0a,0x87,0xd2);
DEFINE_GUID(GUID_WICPixelFormat16bppGray,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x0b);
DEFINE_GUID(GUID_WICPixelFormat24bppBGR,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x0c);
DEFINE_GUID(GUID_WICPixelFormat24bppRGB,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x0d);
DEFINE_GUID(GUID_WICPixelFormat32bppBGR,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x0e);
DEFINE_GUID(GUID_WICPixelFormat32bppBGRA,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x0f);
DEFINE_GUID(GUID_WICPixelFormat32bppPBGRA,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x10);
DEFINE_GUID(GUID_WICPixelFormat32bppGrayFloat,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x11);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
DEFINE_GUID(GUID_WICPixelFormat32bppRGB,0xd98c6b95,0x3efe,0x47d6,0xbb,0x25,0xeb,0x17,0x48,0xab,0x0c,0xf1);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */
DEFINE_GUID(GUID_WICPixelFormat32bppRGBA,0xf5c7ad2d,0x6a8d,0x43dd,0xa7,0xa8,0xa2,0x99, 0x35,0x26,0x1a,0xe9);
DEFINE_GUID(GUID_WICPixelFormat32bppPRGBA,0x3cc4a650,0xa527,0x4d37,0xa9,0x16,0x31,0x42,0xc7,0xeb,0xed,0xba);
DEFINE_GUID(GUID_WICPixelFormat48bppRGB,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x15);
DEFINE_GUID(GUID_WICPixelFormat48bppBGR,0xe605a384,0xb468,0x46ce,0xbb,0x2e,0x36,0xf1,0x80,0xe6,0x43,0x13);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
DEFINE_GUID(GUID_WICPixelFormat64bppRGB,0xa1182111,0x186d,0x4d42,0xbc,0x6a,0x9c,0x83,0x03,0xa8,0xdf,0xf9);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */
DEFINE_GUID(GUID_WICPixelFormat64bppRGBA,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x16);
DEFINE_GUID(GUID_WICPixelFormat64bppBGRA,0x1562ff7c,0xd352,0x46f9,0x97,0x9e,0x42,0x97,0x6b,0x79,0x22,0x46);
DEFINE_GUID(GUID_WICPixelFormat64bppPRGBA,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x17);
DEFINE_GUID(GUID_WICPixelFormat64bppPBGRA,0x8c518e8e,0xa4ec,0x468b,0xae,0x70,0xc9,0xa3,0x5a,0x9c,0x55,0x30);
DEFINE_GUID(GUID_WICPixelFormat16bppGrayFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x13);
DEFINE_GUID(GUID_WICPixelFormat32bppBGR101010,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x14);
DEFINE_GUID(GUID_WICPixelFormat48bppRGBFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x12);
DEFINE_GUID(GUID_WICPixelFormat48bppBGRFixedPoint,0x49ca140e,0xcab6,0x493b,0x9d,0xdf,0x60,0x18,0x7c,0x37,0x53,0x2a);
DEFINE_GUID(GUID_WICPixelFormat96bppRGBFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x18);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
DEFINE_GUID(GUID_WICPixelFormat96bppRGBFloat,0xe3fed78f,0xe8db,0x4acf,0x84,0xc1,0xe9,0x7f,0x61,0x36,0xb3,0x27);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */
DEFINE_GUID(GUID_WICPixelFormat128bppRGBAFloat,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x19);
DEFINE_GUID(GUID_WICPixelFormat128bppPRGBAFloat,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x1a);
DEFINE_GUID(GUID_WICPixelFormat128bppRGBFloat,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x1b);
DEFINE_GUID(GUID_WICPixelFormat32bppCMYK,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x1c);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBAFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x1d);
DEFINE_GUID(GUID_WICPixelFormat64bppBGRAFixedPoint,0x356de33c,0x54d2,0x4a23,0xbb,0x4,0x9b,0x7b,0xf9,0xb1,0xd4,0x2d);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x40);
DEFINE_GUID(GUID_WICPixelFormat128bppRGBAFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x1e);
DEFINE_GUID(GUID_WICPixelFormat128bppRGBFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x41);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBAHalf,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x3a);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
DEFINE_GUID(GUID_WICPixelFormat64bppPRGBAHalf,0x58ad26c2,0xc623,0x4d9d,0xb3,0x20,0x38,0x7e,0x49,0xf8,0xc4,0x42);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */
DEFINE_GUID(GUID_WICPixelFormat64bppRGBHalf,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x42);
DEFINE_GUID(GUID_WICPixelFormat48bppRGBHalf,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x3b);
DEFINE_GUID(GUID_WICPixelFormat32bppRGBE,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x3d);
DEFINE_GUID(GUID_WICPixelFormat16bppGrayHalf,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x3e);
DEFINE_GUID(GUID_WICPixelFormat32bppGrayFixedPoint,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x3f);
DEFINE_GUID(GUID_WICPixelFormat32bppRGBA1010102,0x25238D72,0xFCF9,0x4522,0xb5,0x14,0x55,0x78,0xe5,0xad,0x55,0xe0);
DEFINE_GUID(GUID_WICPixelFormat32bppRGBA1010102XR,0x00DE6B9A,0xC101,0x434b,0xb5,0x02,0xd0,0x16,0x5e,0xe1,0x12,0x2c);
DEFINE_GUID(GUID_WICPixelFormat64bppCMYK,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x1f);
DEFINE_GUID(GUID_WICPixelFormat24bpp3Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x20);
DEFINE_GUID(GUID_WICPixelFormat32bpp4Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x21);
DEFINE_GUID(GUID_WICPixelFormat40bpp5Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x22);
DEFINE_GUID(GUID_WICPixelFormat48bpp6Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x23);
DEFINE_GUID(GUID_WICPixelFormat56bpp7Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x24);
DEFINE_GUID(GUID_WICPixelFormat64bpp8Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x25);
DEFINE_GUID(GUID_WICPixelFormat48bpp3Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x26);
DEFINE_GUID(GUID_WICPixelFormat64bpp4Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x27);
DEFINE_GUID(GUID_WICPixelFormat80bpp5Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x28);
DEFINE_GUID(GUID_WICPixelFormat96bpp6Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x29);
DEFINE_GUID(GUID_WICPixelFormat112bpp7Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x2a);
DEFINE_GUID(GUID_WICPixelFormat128bpp8Channels,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x2b);
DEFINE_GUID(GUID_WICPixelFormat40bppCMYKAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x2c);
DEFINE_GUID(GUID_WICPixelFormat80bppCMYKAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x2d);
DEFINE_GUID(GUID_WICPixelFormat32bpp3ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x2e);
DEFINE_GUID(GUID_WICPixelFormat40bpp4ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x2f);
DEFINE_GUID(GUID_WICPixelFormat48bpp5ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x30);
DEFINE_GUID(GUID_WICPixelFormat56bpp6ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x31);
DEFINE_GUID(GUID_WICPixelFormat64bpp7ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x32);
DEFINE_GUID(GUID_WICPixelFormat72bpp8ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x33);
DEFINE_GUID(GUID_WICPixelFormat64bpp3ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x34);
DEFINE_GUID(GUID_WICPixelFormat80bpp4ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x35);
DEFINE_GUID(GUID_WICPixelFormat96bpp5ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x36);
DEFINE_GUID(GUID_WICPixelFormat112bpp6ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x37);
DEFINE_GUID(GUID_WICPixelFormat128bpp7ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x38);
DEFINE_GUID(GUID_WICPixelFormat144bpp8ChannelsAlpha,0x6fddc324,0x4e03,0x4bfe,0xb1,0x85,0x3d,0x77,0x76,0x8d,0xc9,0x39);
DEFINE_GUID(GUID_WICPixelFormat8bppY,0x91B4DB54,0x2DF9,0x42F0,0xB4,0x49,0x29,0x09,0xBB,0x3D,0xF8,0x8E);
DEFINE_GUID(GUID_WICPixelFormat8bppCb,0x1339F224,0x6BFE,0x4C3E,0x93,0x02,0xE4,0xF3,0xA6,0xD0,0xCA,0x2A);
DEFINE_GUID(GUID_WICPixelFormat8bppCr,0xB8145053,0x2116,0x49F0,0x88,0x35,0xED,0x84,0x4B,0x20,0x5C,0x51);
DEFINE_GUID(GUID_WICPixelFormat16bppCbCr,0xFF95BA6E,0x11E0,0x4263,0xBB,0x45,0x01,0x72,0x1F,0x34,0x60,0xA4);

typedef enum WICBitmapCreateCacheOption {
    WICBitmapNoCache = 0,
    WICBitmapCacheOnDemand = 0x1,
    WICBitmapCacheOnLoad = 0x2,
    WICBITMAPCREATECACHEOPTION_FORCE_DWORD = 0x7fffffff
} WICBitmapCreateCacheOption;

typedef enum WICDecodeOptions {
    WICDecodeMetadataCacheOnDemand = 0,
    WICDecodeMetadataCacheOnLoad = 0x1,
    WICMETADATACACHEOPTION_FORCE_DWORD = 0x7fffffff
} WICDecodeOptions;

typedef enum WICBitmapEncoderCacheOption {
    WICBitmapEncoderCacheInMemory = 0,
    WICBitmapEncoderCacheTempFile = 0x1,
    WICBitmapEncoderNoCache = 0x2,
    WICBITMAPENCODERCACHEOPTION_FORCE_DWORD = 0x7fffffff
} WICBitmapEncoderCacheOption;

typedef enum WICComponentType {
    WICDecoder = 0x1,
    WICEncoder = 0x2,
    WICPixelFormatConverter = 0x4,
    WICMetadataReader = 0x8,
    WICMetadataWriter = 0x10,
    WICPixelFormat = 0x20,
    WICAllComponents = 0x3f,
    WICCOMPONENTTYPE_FORCE_DWORD = 0x7fffffff
} WICComponentType;

typedef enum WICComponentEnumerateOptions {
    WICComponentEnumerateDefault = 0,
    WICComponentEnumerateRefresh = 0x1,
    WICComponentEnumerateDisabled = 0x80000000,
    WICComponentEnumerateUnsigned = 0x40000000,
    WICCOMPONENTENUMERATEOPTIONS_FORCE_DWORD = 0x7fffffff
} WICComponentEnumerateOptions;

typedef struct WICBitmapPattern {
    ULARGE_INTEGER Position;
    ULONG Length;
    BYTE *Pattern;
    BYTE *Mask;
    BOOL EndOfStream;
} WICBitmapPattern;

typedef enum WICBitmapInterpolationMode {
    WICBitmapInterpolationModeNearestNeighbor = 0,
    WICBitmapInterpolationModeLinear = 0x1,
    WICBitmapInterpolationModeCubic = 0x2,
    WICBitmapInterpolationModeFant = 0x3,
    WICBITMAPINTERPOLATIONMODE_FORCE_DWORD = 0x7fffffff
} WICBitmapInterpolationMode;

typedef enum WICBitmapPaletteType {
    WICBitmapPaletteTypeCustom = 0,
    WICBitmapPaletteTypeMedianCut = 0x1,
    WICBitmapPaletteTypeFixedBW = 0x2,
    WICBitmapPaletteTypeFixedHalftone8 = 0x3,
    WICBitmapPaletteTypeFixedHalftone27 = 0x4,
    WICBitmapPaletteTypeFixedHalftone64 = 0x5,
    WICBitmapPaletteTypeFixedHalftone125 = 0x6,
    WICBitmapPaletteTypeFixedHalftone216 = 0x7,
    WICBitmapPaletteTypeFixedWebPalette = WICBitmapPaletteTypeFixedHalftone216,
    WICBitmapPaletteTypeFixedHalftone252 = 0x8,
    WICBitmapPaletteTypeFixedHalftone256 = 0x9,
    WICBitmapPaletteTypeFixedGray4 = 0xa,
    WICBitmapPaletteTypeFixedGray16 = 0xb,
    WICBitmapPaletteTypeFixedGray256 = 0xc,
    WICBITMAPPALETTETYPE_FORCE_DWORD = 0x7fffffff
} WICBitmapPaletteType;

typedef enum WICBitmapDitherType {
    WICBitmapDitherTypeNone = 0,
    WICBitmapDitherTypeSolid = 0,
    WICBitmapDitherTypeOrdered4x4 = 0x1,
    WICBitmapDitherTypeOrdered8x8 = 0x2,
    WICBitmapDitherTypeOrdered16x16 = 0x3,
    WICBitmapDitherTypeSpiral4x4 = 0x4,
    WICBitmapDitherTypeSpiral8x8 = 0x5,
    WICBitmapDitherTypeDualSpiral4x4 = 0x6,
    WICBitmapDitherTypeDualSpiral8x8 = 0x7,
    WICBitmapDitherTypeErrorDiffusion = 0x8,
    WICBITMAPDITHERTYPE_FORCE_DWORD = 0x7fffffff
} WICBitmapDitherType;

typedef enum WICBitmapAlphaChannelOption {
    WICBitmapUseAlpha = 0,
    WICBitmapUsePremultipliedAlpha = 0x1,
    WICBitmapIgnoreAlpha = 0x2,
    WICBITMAPALPHACHANNELOPTIONS_FORCE_DWORD = 0x7fffffff
} WICBitmapAlphaChannelOption;

typedef enum WICBitmapTransformOptions {
    WICBitmapTransformRotate0 = 0,
    WICBitmapTransformRotate90 = 0x1,
    WICBitmapTransformRotate180 = 0x2,
    WICBitmapTransformRotate270 = 0x3,
    WICBitmapTransformFlipHorizontal = 0x8,
    WICBitmapTransformFlipVertical = 0x10,
    WICBITMAPTRANSFORMOPTIONS_FORCE_DWORD = 0x7fffffff
} WICBitmapTransformOptions;

typedef enum WICBitmapLockFlags {
    WICBitmapLockRead = 0x1,
    WICBitmapLockWrite = 0x2,
    WICBITMAPLOCKFLAGS_FORCE_DWORD = 0x7fffffff
} WICBitmapLockFlags;

typedef enum WICBitmapDecoderCapabilities {
    WICBitmapDecoderCapabilitySameEncoder = 0x1,
    WICBitmapDecoderCapabilityCanDecodeAllImages = 0x2,
    WICBitmapDecoderCapabilityCanDecodeSomeImages = 0x4,
    WICBitmapDecoderCapabilityCanEnumerateMetadata = 0x8,
    WICBitmapDecoderCapabilityCanDecodeThumbnail = 0x10,
    WICBITMAPDECODERCAPABILITIES_FORCE_DWORD = 0x7fffffff
} WICBitmapDecoderCapabilities;

typedef enum WICProgressOperation {
    WICProgressOperationCopyPixels = 0x1,
    WICProgressOperationWritePixels = 0x2,
    WICProgressOperationAll = 0xffff,
    WICPROGRESSOPERATION_FORCE_DWORD = 0x7fffffff
} WICProgressOperation;

typedef enum WICProgressNotification {
    WICProgressNotificationBegin = 0x10000,
    WICProgressNotificationEnd = 0x20000,
    WICProgressNotificationFrequent = 0x40000,
    WICProgressNotificationAll = 0xffff0000,
    WICPROGRESSNOTIFICATION_FORCE_DWORD = 0x7fffffff
} WICProgressNotification;

typedef enum WICComponentSigning {
    WICComponentSigned = 0x1,
    WICComponentUnsigned = 0x2,
    WICComponentSafe = 0x4,
    WICComponentDisabled = 0x80000000,
    WICCOMPONENTSIGNING_FORCE_DWORD = 0x7fffffff
} WICComponentSigning;

typedef enum WICGifLogicalScreenDescriptorProperties {
    WICGifLogicalScreenSignature = 0x1,
    WICGifLogicalScreenDescriptorWidth = 0x2,
    WICGifLogicalScreenDescriptorHeight = 0x3,
    WICGifLogicalScreenDescriptorGlobalColorTableFlag = 0x4,
    WICGifLogicalScreenDescriptorColorResolution = 0x5,
    WICGifLogicalScreenDescriptorSortFlag = 0x6,
    WICGifLogicalScreenDescriptorGlobalColorTableSize = 0x7,
    WICGifLogicalScreenDescriptorBackgroundColorIndex = 0x8,
    WICGifLogicalScreenDescriptorPixelAspectRatio = 0x9,
    WICGifLogicalScreenDescriptorProperties_FORCE_DWORD = 0x7fffffff
} WICGifLogicalScreenDescriptorProperties;

typedef enum WICGifImageDescriptorProperties {
    WICGifImageDescriptorLeft = 0x1,
    WICGifImageDescriptorTop = 0x2,
    WICGifImageDescriptorWidth = 0x3,
    WICGifImageDescriptorHeight = 0x4,
    WICGifImageDescriptorLocalColorTableFlag = 0x5,
    WICGifImageDescriptorInterlaceFlag = 0x6,
    WICGifImageDescriptorSortFlag = 0x7,
    WICGifImageDescriptorLocalColorTableSize = 0x8,
    WICGifImageDescriptorProperties_FORCE_DWORD = 0x7fffffff
} WICGifImageDescriptorProperties;

typedef enum WICGifGraphicControlExtensionProperties {
    WICGifGraphicControlExtensionDisposal = 0x1,
    WICGifGraphicControlExtensionUserInputFlag = 0x2,
    WICGifGraphicControlExtensionTransparencyFlag = 0x3,
    WICGifGraphicControlExtensionDelay = 0x4,
    WICGifGraphicControlExtensionTransparentColorIndex = 0x5,
    WICGifGraphicControlExtensionProperties_FORCE_DWORD = 0x7fffffff
} WICGifGraphicControlExtensionProperties;

typedef enum WICGifApplicationExtensionProperties {
    WICGifApplicationExtensionApplication = 0x1,
    WICGifApplicationExtensionData = 0x2,
    WICGifApplicationExtensionProperties_FORCE_DWORD = 0x7fffffff
} WICGifApplicationExtensionProperties;

typedef enum WICGifCommentExtensionProperties {
    WICGifCommentExtensionText = 0x1,
    WICGifCommentExtensionProperties_FORCE_DWORD = 0x7fffffff
} WICGifCommentExtensionProperties;

typedef enum WICJpegCommentProperties {
    WICJpegCommentText = 0x1,
    WICJpegCommentProperties_FORCE_DWORD = 0x7fffffff
} WICJpegCommentProperties;

typedef enum WICJpegLuminanceProperties {
    WICJpegLuminanceTable = 0x1,
    WICJpegLuminanceProperties_FORCE_DWORD = 0x7fffffff
} WICJpegLuminanceProperties;

typedef enum WICJpegChrominanceProperties {
    WICJpegChrominanceTable = 0x1,
    WICJpegChrominanceProperties_FORCE_DWORD = 0x7fffffff
} WICJpegChrominanceProperties;

typedef enum WIC8BIMIptcProperties {
    WIC8BIMIptcPString = 0,
    WIC8BIMIptcEmbeddedIPTC = 0x1,
    WIC8BIMIptcProperties_FORCE_DWORD = 0x7fffffff
} WIC8BIMIptcProperties;

typedef enum WIC8BIMResolutionInfoProperties {
    WIC8BIMResolutionInfoPString = 0x1,
    WIC8BIMResolutionInfoHResolution = 0x2,
    WIC8BIMResolutionInfoHResolutionUnit = 0x3,
    WIC8BIMResolutionInfoWidthUnit = 0x4,
    WIC8BIMResolutionInfoVResolution = 0x5,
    WIC8BIMResolutionInfoVResolutionUnit = 0x6,
    WIC8BIMResolutionInfoHeightUnit = 0x7,
    WIC8BIMResolutionInfoProperties_FORCE_DWORD = 0x7fffffff
} WIC8BIMResolutionInfoProperties;

typedef enum WIC8BIMIptcDigestProperties {
    WIC8BIMIptcDigestPString = 0x1,
    WIC8BIMIptcDigestIptcDigest = 0x2,
    WIC8BIMIptcDigestProperties_FORCE_DWORD = 0x7fffffff
} WIC8BIMIptcDigestProperties;

typedef enum WICPngGamaProperties {
    WICPngGamaGamma = 0x1,
    WICPngGamaProperties_FORCE_DWORD = 0x7fffffff
} WICPngGamaProperties;

typedef enum WICPngBkgdProperties {
    WICPngBkgdBackgroundColor = 0x1,
    WICPngBkgdProperties_FORCE_DWORD = 0x7fffffff
} WICPngBkgdProperties;

typedef
    enum WICPngItxtProperties {
    WICPngItxtKeyword = 0x1,
    WICPngItxtCompressionFlag = 0x2,
    WICPngItxtLanguageTag = 0x3,
    WICPngItxtTranslatedKeyword = 0x4,
    WICPngItxtText = 0x5,
    WICPngItxtProperties_FORCE_DWORD = 0x7fffffff
} WICPngItxtProperties;

typedef enum WICPngChrmProperties {
    WICPngChrmWhitePointX = 0x1,
    WICPngChrmWhitePointY = 0x2,
    WICPngChrmRedX = 0x3,
    WICPngChrmRedY = 0x4,
    WICPngChrmGreenX = 0x5,
    WICPngChrmGreenY = 0x6,
    WICPngChrmBlueX = 0x7,
    WICPngChrmBlueY = 0x8,
    WICPngChrmProperties_FORCE_DWORD = 0x7fffffff
} WICPngChrmProperties;

typedef enum WICPngHistProperties {
    WICPngHistFrequencies = 0x1,
    WICPngHistProperties_FORCE_DWORD = 0x7fffffff
} WICPngHistProperties;

typedef enum WICPngIccpProperties {
    WICPngIccpProfileName = 0x1,
    WICPngIccpProfileData = 0x2,
    WICPngIccpProperties_FORCE_DWORD = 0x7fffffff
} WICPngIccpProperties;

typedef enum WICPngSrgbProperties {
    WICPngSrgbRenderingIntent = 0x1,
    WICPngSrgbProperties_FORCE_DWORD = 0x7fffffff
} WICPngSrgbProperties;

typedef enum WICPngTimeProperties {
    WICPngTimeYear = 0x1,
    WICPngTimeMonth = 0x2,
    WICPngTimeDay = 0x3,
    WICPngTimeHour = 0x4,
    WICPngTimeMinute = 0x5,
    WICPngTimeSecond = 0x6,
    WICPngTimeProperties_FORCE_DWORD = 0x7fffffff
} WICPngTimeProperties;

typedef enum WICSectionAccessLevel {
    WICSectionAccessLevelRead = 0x1,
    WICSectionAccessLevelReadWrite = 0x3,
    WICSectionAccessLevel_FORCE_DWORD = 0x7fffffff
} WICSectionAccessLevel;

typedef enum WICPixelFormatNumericRepresentation {
    WICPixelFormatNumericRepresentationUnspecified = 0,
    WICPixelFormatNumericRepresentationIndexed = 0x1,
    WICPixelFormatNumericRepresentationUnsignedInteger = 0x2,
    WICPixelFormatNumericRepresentationSignedInteger = 0x3,
    WICPixelFormatNumericRepresentationFixed = 0x4,
    WICPixelFormatNumericRepresentationFloat = 0x5,
    WICPixelFormatNumericRepresentation_FORCE_DWORD = 0x7fffffff
} WICPixelFormatNumericRepresentation;

typedef enum WICPlanarOptions {
    WICPlanarOptionsDefault = 0,
    WICPlanarOptionsPreserveSubsampling = 0x1,
    WICPLANAROPTIONS_FORCE_DWORD = 0x7fffffff
} WICPlanarOptions;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
typedef struct WICImageParameters {
    D2D1_PIXEL_FORMAT PixelFormat;
    FLOAT DpiX;
    FLOAT DpiY;
    FLOAT Top;
    FLOAT Left;
    UINT32 PixelWidth;
    UINT32 PixelHeight;
} WICImageParameters;
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */

typedef struct WICBitmapPlaneDescription {
    WICPixelFormatGUID Format;
    UINT Width;
    UINT Height;
} WICBitmapPlaneDescription;

typedef struct WICBitmapPlane {
    WICPixelFormatGUID Format;
    BYTE *pbBuffer;
    UINT cbStride;
    UINT cbBufferSize;
} WICBitmapPlane;

extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0000_v0_0_s_ifspec;

#ifndef __IWICPalette_INTERFACE_DEFINED__
#define __IWICPalette_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICPalette;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000040-a8f2-4877-ba0a-fd2b6645fb94")IWICPalette:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InitializePredefined(WICBitmapPaletteType ePaletteType, BOOL fAddTransparentColor) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeCustom(WICColor * pColors, UINT cCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeFromBitmap(IWICBitmapSource * pISurface, UINT cCount, BOOL fAddTransparentColor) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeFromPalette(IWICPalette * pIPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetType(WICBitmapPaletteType * pePaletteType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColorCount(UINT * pcCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColors(UINT cCount, WICColor * pColors, UINT * pcActualColors) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsBlackWhite(BOOL * pfIsBlackWhite) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsGrayscale(BOOL * pfIsGrayscale) = 0;
    virtual HRESULT STDMETHODCALLTYPE HasAlpha(BOOL * pfHasAlpha) = 0;
};
#else /* C style interface */
typedef struct IWICPaletteVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICPalette * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICPalette * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICPalette * This);
    HRESULT(STDMETHODCALLTYPE *InitializePredefined)(IWICPalette * This, WICBitmapPaletteType ePaletteType, BOOL fAddTransparentColor);
    HRESULT(STDMETHODCALLTYPE *InitializeCustom)(IWICPalette * This, WICColor * pColors, UINT cCount);
    HRESULT(STDMETHODCALLTYPE *InitializeFromBitmap)(IWICPalette * This, IWICBitmapSource * pISurface, UINT cCount, BOOL fAddTransparentColor);
    HRESULT(STDMETHODCALLTYPE *InitializeFromPalette)(IWICPalette * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *GetType)(IWICPalette * This, WICBitmapPaletteType * pePaletteType);
    HRESULT(STDMETHODCALLTYPE *GetColorCount)(IWICPalette * This, UINT * pcCount);
    HRESULT(STDMETHODCALLTYPE *GetColors)(IWICPalette * This, UINT cCount, WICColor * pColors, UINT * pcActualColors);
    HRESULT(STDMETHODCALLTYPE *IsBlackWhite)(IWICPalette * This, BOOL * pfIsBlackWhite);
    HRESULT(STDMETHODCALLTYPE *IsGrayscale)(IWICPalette * This, BOOL * pfIsGrayscale);
    HRESULT(STDMETHODCALLTYPE *HasAlpha)(IWICPalette * This, BOOL * pfHasAlpha);
    END_INTERFACE
} IWICPaletteVtbl;

interface IWICPalette {
    CONST_VTBL struct IWICPaletteVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICPalette_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICPalette_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICPalette_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICPalette_InitializePredefined(This,ePaletteType,fAddTransparentColor)  ((This)->lpVtbl->InitializePredefined(This,ePaletteType,fAddTransparentColor))
#define IWICPalette_InitializeCustom(This,pColors,cCount)  ((This)->lpVtbl->InitializeCustom(This,pColors,cCount))
#define IWICPalette_InitializeFromBitmap(This,pISurface,cCount,fAddTransparentColor)  ((This)->lpVtbl->InitializeFromBitmap(This,pISurface,cCount,fAddTransparentColor))
#define IWICPalette_InitializeFromPalette(This,pIPalette)  ((This)->lpVtbl->InitializeFromPalette(This,pIPalette))
#define IWICPalette_GetType(This,pePaletteType)  ((This)->lpVtbl->GetType(This,pePaletteType))
#define IWICPalette_GetColorCount(This,pcCount)  ((This)->lpVtbl->GetColorCount(This,pcCount))
#define IWICPalette_GetColors(This,cCount,pColors,pcActualColors)  ((This)->lpVtbl->GetColors(This,cCount,pColors,pcActualColors))
#define IWICPalette_IsBlackWhite(This,pfIsBlackWhite)  ((This)->lpVtbl->IsBlackWhite(This,pfIsBlackWhite))
#define IWICPalette_IsGrayscale(This,pfIsGrayscale)   ((This)->lpVtbl->IsGrayscale(This,pfIsGrayscale))
#define IWICPalette_HasAlpha(This,pfHasAlpha)  ((This)->lpVtbl->HasAlpha(This,pfHasAlpha))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICPalette_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapSource_INTERFACE_DEFINED__
#define __IWICBitmapSource_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000120-a8f2-4877-ba0a-fd2b6645fb94")IWICBitmapSource:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSize(UINT * puiWidth, UINT * puiHeight) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPixelFormat(WICPixelFormatGUID * pPixelFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResolution(double *pDpiX, double *pDpiY) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyPalette(IWICPalette * pIPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyPixels(const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapSourceVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapSource * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapSource * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapSource * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICBitmapSource * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICBitmapSource * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICBitmapSource * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICBitmapSource * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICBitmapSource * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    END_INTERFACE
} IWICBitmapSourceVtbl;

interface IWICBitmapSource {
    CONST_VTBL struct IWICBitmapSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapSource_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapSource_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICBitmapSource_Release(This)    ((This)->lpVtbl->Release(This))
#define IWICBitmapSource_GetSize(This,puiWidth,puiHeight)  ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICBitmapSource_GetPixelFormat(This,pPixelFormat)    ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICBitmapSource_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICBitmapSource_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICBitmapSource_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapSource_INTERFACE_DEFINED__ */

#ifndef __IWICFormatConverter_INTERFACE_DEFINED__
#define __IWICFormatConverter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICFormatConverter;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000301-a8f2-4877-ba0a-fd2b6645fb94")IWICFormatConverter:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IWICBitmapSource * pISource, REFWICPixelFormatGUID dstFormat, WICBitmapDitherType dither, IWICPalette * pIPalette, double alphaThresholdPercent, WICBitmapPaletteType paletteTranslate) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanConvert(REFWICPixelFormatGUID srcPixelFormat, REFWICPixelFormatGUID dstPixelFormat, BOOL * pfCanConvert) = 0;
};
#else /* C style interface */
typedef struct IWICFormatConverterVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICFormatConverter * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICFormatConverter * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICFormatConverter * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICFormatConverter * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICFormatConverter * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICFormatConverter * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICFormatConverter * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICFormatConverter * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICFormatConverter * This, IWICBitmapSource * pISource, REFWICPixelFormatGUID dstFormat, WICBitmapDitherType dither, IWICPalette * pIPalette, double alphaThresholdPercent, WICBitmapPaletteType paletteTranslate);
    HRESULT(STDMETHODCALLTYPE *CanConvert)(IWICFormatConverter * This, REFWICPixelFormatGUID srcPixelFormat, REFWICPixelFormatGUID dstPixelFormat, BOOL * pfCanConvert);
    END_INTERFACE
} IWICFormatConverterVtbl;

interface IWICFormatConverter {
    CONST_VTBL struct IWICFormatConverterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICFormatConverter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICFormatConverter_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICFormatConverter_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICFormatConverter_GetSize(This,puiWidth,puiHeight)  ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICFormatConverter_GetPixelFormat(This,pPixelFormat)  ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICFormatConverter_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICFormatConverter_CopyPalette(This,pIPalette)   ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICFormatConverter_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)   ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICFormatConverter_Initialize(This,pISource,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate)   ((This)->lpVtbl->Initialize(This,pISource,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate))
#define IWICFormatConverter_CanConvert(This,srcPixelFormat,dstPixelFormat,pfCanConvert)   ((This)->lpVtbl->CanConvert(This,srcPixelFormat,dstPixelFormat,pfCanConvert))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICFormatConverter_INTERFACE_DEFINED__ */

#ifndef __IWICPlanarFormatConverter_INTERFACE_DEFINED__
#define __IWICPlanarFormatConverter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICPlanarFormatConverter;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("BEBEE9CB-83B0-4DCC-8132-B0AAA55EAC96")IWICPlanarFormatConverter:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IWICBitmapSource **, UINT, REFWICPixelFormatGUID, WICBitmapDitherType, IWICPalette *, double, WICBitmapPaletteType) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanConvert(const WICPixelFormatGUID *, UINT, REFWICPixelFormatGUID, BOOL *) = 0;
};
#else /* C style interface */
typedef struct IWICPlanarFormatConverterVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWICPlanarFormatConverter *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICPlanarFormatConverter *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICPlanarFormatConverter *);
    HRESULT (STDMETHODCALLTYPE *GetSize)(IWICPlanarFormatConverter *, UINT *, UINT *);
    HRESULT (STDMETHODCALLTYPE *GetPixelFormat)(IWICPlanarFormatConverter *, WICPixelFormatGUID *);
    HRESULT (STDMETHODCALLTYPE *GetResolution)(IWICPlanarFormatConverter *, double *, double *);
    HRESULT (STDMETHODCALLTYPE *CopyPalette)(IWICPlanarFormatConverter *, IWICPalette *);
    HRESULT (STDMETHODCALLTYPE *CopyPixels)(IWICPlanarFormatConverter *, const WICRect *, UINT, UINT, BYTE *);
    HRESULT (STDMETHODCALLTYPE *Initialize)(IWICPlanarFormatConverter *, IWICBitmapSource **, UINT, REFWICPixelFormatGUID, WICBitmapDitherType, IWICPalette *, double, WICBitmapPaletteType);
    HRESULT (STDMETHODCALLTYPE *CanConvert)(IWICPlanarFormatConverter *, const WICPixelFormatGUID *, UINT, REFWICPixelFormatGUID, BOOL *);
    END_INTERFACE
} IWICPlanarFormatConverterVtbl;

interface IWICPlanarFormatConverter {
    CONST_VTBL struct IWICPlanarFormatConverterVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IWICPlanarFormatConverter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICPlanarFormatConverter_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICPlanarFormatConverter_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICPlanarFormatConverter_GetSize(This,puiWidth,puiHeight)  ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICPlanarFormatConverter_GetPixelFormat(This,pPixelFormat)  ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICPlanarFormatConverter_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICPlanarFormatConverter_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICPlanarFormatConverter_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICPlanarFormatConverter_Initialize(This,ppPlanes,cPlanes,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate)  ((This)->lpVtbl->Initialize(This,ppPlanes,cPlanes,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate))
#define IWICPlanarFormatConverter_CanConvert(This,pSrcPixelFormats,cSrcPlanes,dstPixelFormat,pfCanConvert)  ((This)->lpVtbl->CanConvert(This,pSrcPixelFormats,cSrcPlanes,dstPixelFormat,pfCanConvert))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICPlanarFormatConverter_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapScaler_INTERFACE_DEFINED__
#define __IWICBitmapScaler_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapScaler;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000302-a8f2-4877-ba0a-fd2b6645fb94")IWICBitmapScaler:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IWICBitmapSource * pISource, UINT uiWidth, UINT uiHeight, WICBitmapInterpolationMode mode) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapScalerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapScaler * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapScaler * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapScaler * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICBitmapScaler * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICBitmapScaler * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICBitmapScaler * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICBitmapScaler * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICBitmapScaler * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICBitmapScaler * This, IWICBitmapSource * pISource, UINT uiWidth, UINT uiHeight, WICBitmapInterpolationMode mode);
    END_INTERFACE
} IWICBitmapScalerVtbl;

interface IWICBitmapScaler {
    CONST_VTBL struct IWICBitmapScalerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapScaler_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapScaler_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICBitmapScaler_Release(This)    ((This)->lpVtbl->Release(This))
#define IWICBitmapScaler_GetSize(This,puiWidth,puiHeight)  ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICBitmapScaler_GetPixelFormat(This,pPixelFormat)    ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICBitmapScaler_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICBitmapScaler_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICBitmapScaler_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICBitmapScaler_Initialize(This,pISource,uiWidth,uiHeight,mode)  ((This)->lpVtbl->Initialize(This,pISource,uiWidth,uiHeight,mode))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapScaler_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapClipper_INTERFACE_DEFINED__
#define __IWICBitmapClipper_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapClipper;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("E4FBCF03-223D-4e81-9333-D635556DD1B5")IWICBitmapClipper:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IWICBitmapSource * pISource, const WICRect * prc) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapClipperVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapClipper * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapClipper * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapClipper * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICBitmapClipper * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICBitmapClipper * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICBitmapClipper * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICBitmapClipper * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICBitmapClipper * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICBitmapClipper * This, IWICBitmapSource * pISource, const WICRect * prc);
    END_INTERFACE
} IWICBitmapClipperVtbl;

interface IWICBitmapClipper {
    CONST_VTBL struct IWICBitmapClipperVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapClipper_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapClipper_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapClipper_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapClipper_GetSize(This,puiWidth,puiHeight)    ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICBitmapClipper_GetPixelFormat(This,pPixelFormat)   ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICBitmapClipper_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICBitmapClipper_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICBitmapClipper_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICBitmapClipper_Initialize(This,pISource,prc)   ((This)->lpVtbl->Initialize(This,pISource,prc))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapClipper_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapFlipRotator_INTERFACE_DEFINED__
#define __IWICBitmapFlipRotator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapFlipRotator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5009834F-2D6A-41ce-9E1B-17C5AFF7A782")IWICBitmapFlipRotator:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IWICBitmapSource * pISource, WICBitmapTransformOptions options) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapFlipRotatorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapFlipRotator * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapFlipRotator * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapFlipRotator * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICBitmapFlipRotator * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICBitmapFlipRotator * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICBitmapFlipRotator * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICBitmapFlipRotator * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICBitmapFlipRotator * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICBitmapFlipRotator * This, IWICBitmapSource * pISource, WICBitmapTransformOptions options);
    END_INTERFACE
} IWICBitmapFlipRotatorVtbl;

interface IWICBitmapFlipRotator {
    CONST_VTBL struct IWICBitmapFlipRotatorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapFlipRotator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapFlipRotator_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapFlipRotator_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapFlipRotator_GetSize(This,puiWidth,puiHeight)    ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICBitmapFlipRotator_GetPixelFormat(This,pPixelFormat)   ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICBitmapFlipRotator_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICBitmapFlipRotator_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICBitmapFlipRotator_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICBitmapFlipRotator_Initialize(This,pISource,options)   ((This)->lpVtbl->Initialize(This,pISource,options))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapFlipRotator_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapLock_INTERFACE_DEFINED__
#define __IWICBitmapLock_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapLock;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000123-a8f2-4877-ba0a-fd2b6645fb94")IWICBitmapLock:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSize(UINT * puiWidth, UINT * puiHeight) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStride(UINT * pcbStride) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDataPointer(UINT * pcbBufferSize, BYTE ** ppbData) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPixelFormat(WICPixelFormatGUID * pPixelFormat) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapLockVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapLock * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapLock * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapLock * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICBitmapLock * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetStride)(IWICBitmapLock * This, UINT * pcbStride);
    HRESULT(STDMETHODCALLTYPE *GetDataPointer)(IWICBitmapLock * This, UINT * pcbBufferSize, BYTE ** ppbData);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICBitmapLock * This, WICPixelFormatGUID * pPixelFormat);
    END_INTERFACE
} IWICBitmapLockVtbl;

interface IWICBitmapLock {
    CONST_VTBL struct IWICBitmapLockVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapLock_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapLock_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICBitmapLock_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICBitmapLock_GetSize(This,puiWidth,puiHeight)  ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICBitmapLock_GetStride(This,pcbStride)  ((This)->lpVtbl->GetStride(This,pcbStride))
#define IWICBitmapLock_GetDataPointer(This,pcbBufferSize,ppbData)  ((This)->lpVtbl->GetDataPointer(This,pcbBufferSize,ppbData))
#define IWICBitmapLock_GetPixelFormat(This,pPixelFormat)  ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWICBitmapLock_remote_shouldnt_be_called_GetDataPointer_Proxy(IWICBitmapLock * This, UINT * pcbBufferSize, BYTE ** ppbData);
void __RPC_STUB IWICBitmapLock_remote_shouldnt_be_called_GetDataPointer_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWICBitmapLock_INTERFACE_DEFINED__ */

#ifndef __IWICBitmap_INTERFACE_DEFINED__
#define __IWICBitmap_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmap;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000121-a8f2-4877-ba0a-fd2b6645fb94")IWICBitmap:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Lock(const WICRect * prcLock, DWORD flags, IWICBitmapLock ** ppILock) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPalette(IWICPalette * pIPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetResolution(double dpiX, double dpiY) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmap * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmap * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmap * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICBitmap * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICBitmap * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICBitmap * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICBitmap * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICBitmap * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *Lock)(IWICBitmap * This, const WICRect * prcLock, DWORD flags, IWICBitmapLock ** ppILock);
    HRESULT(STDMETHODCALLTYPE *SetPalette)(IWICBitmap * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *SetResolution)(IWICBitmap * This, double dpiX, double dpiY);
    END_INTERFACE
} IWICBitmapVtbl;

interface IWICBitmap {
    CONST_VTBL struct IWICBitmapVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmap_QueryInterface(This,riid,ppvObject)    ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmap_AddRef(This)   ((This)->lpVtbl->AddRef(This))
#define IWICBitmap_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICBitmap_GetSize(This,puiWidth,puiHeight)   ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICBitmap_GetPixelFormat(This,pPixelFormat)  ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICBitmap_GetResolution(This,pDpiX,pDpiY)    ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICBitmap_CopyPalette(This,pIPalette)    ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICBitmap_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)    ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICBitmap_Lock(This,prcLock,flags,ppILock)   ((This)->lpVtbl->Lock(This,prcLock,flags,ppILock))
#define IWICBitmap_SetPalette(This,pIPalette)  ((This)->lpVtbl->SetPalette(This,pIPalette))
#define IWICBitmap_SetResolution(This,dpiX,dpiY)  ((This)->lpVtbl->SetResolution(This,dpiX,dpiY))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmap_INTERFACE_DEFINED__ */


#ifndef __IWICColorContext_INTERFACE_DEFINED__
#define __IWICColorContext_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICColorContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3C613A02-34B2-44ea-9A7C-45AEA9C6FD6D")IWICColorContext:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InitializeFromFilename(LPCWSTR wzFilename) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeFromMemory(const BYTE * pbBuffer, UINT cbBufferSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeFromExifColorSpace(UINT value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetType(WICColorContextType * pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProfileBytes(UINT cbBuffer, BYTE * pbBuffer, UINT * pcbActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExifColorSpace(UINT * pValue) = 0;
};
#else /* C style interface */
typedef struct IWICColorContextVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICColorContext * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICColorContext * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICColorContext * This);
    HRESULT(STDMETHODCALLTYPE *InitializeFromFilename)(IWICColorContext * This, LPCWSTR wzFilename);
    HRESULT(STDMETHODCALLTYPE *InitializeFromMemory)(IWICColorContext * This, const BYTE * pbBuffer, UINT cbBufferSize);
    HRESULT(STDMETHODCALLTYPE *InitializeFromExifColorSpace)(IWICColorContext * This, UINT value);
    HRESULT(STDMETHODCALLTYPE *GetType)(IWICColorContext * This, WICColorContextType * pType);
    HRESULT(STDMETHODCALLTYPE *GetProfileBytes)(IWICColorContext * This, UINT cbBuffer, BYTE * pbBuffer, UINT * pcbActual);
    HRESULT(STDMETHODCALLTYPE *GetExifColorSpace)(IWICColorContext * This, UINT * pValue);
    END_INTERFACE
} IWICColorContextVtbl;

interface IWICColorContext {
    CONST_VTBL struct IWICColorContextVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICColorContext_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICColorContext_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICColorContext_Release(This)    ((This)->lpVtbl->Release(This))
#define IWICColorContext_InitializeFromFilename(This,wzFilename)  ((This)->lpVtbl->InitializeFromFilename(This,wzFilename))
#define IWICColorContext_InitializeFromMemory(This,pbBuffer,cbBufferSize)  ((This)->lpVtbl->InitializeFromMemory(This,pbBuffer,cbBufferSize))
#define IWICColorContext_InitializeFromExifColorSpace(This,value)  ((This)->lpVtbl->InitializeFromExifColorSpace(This,value))
#define IWICColorContext_GetType(This,pType)  ((This)->lpVtbl->GetType(This,pType))
#define IWICColorContext_GetProfileBytes(This,cbBuffer,pbBuffer,pcbActual)    ((This)->lpVtbl->GetProfileBytes(This,cbBuffer,pbBuffer,pcbActual))
#define IWICColorContext_GetExifColorSpace(This,pValue)   ((This)->lpVtbl->GetExifColorSpace(This,pValue))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICColorContext_INTERFACE_DEFINED__ */


#ifndef __IWICColorTransform_INTERFACE_DEFINED__
#define __IWICColorTransform_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICColorTransform;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("B66F034F-D0E2-40ab-B436-6DE39E321A94")IWICColorTransform:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IWICBitmapSource * pIBitmapSource, IWICColorContext * pIContextSource, IWICColorContext * pIContextDest, REFWICPixelFormatGUID pixelFmtDest) = 0;
};
#else /* C style interface */
typedef struct IWICColorTransformVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICColorTransform * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICColorTransform * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICColorTransform * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICColorTransform * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICColorTransform * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICColorTransform * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICColorTransform * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICColorTransform * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICColorTransform * This, IWICBitmapSource * pIBitmapSource, IWICColorContext * pIContextSource, IWICColorContext * pIContextDest, REFWICPixelFormatGUID pixelFmtDest);
    END_INTERFACE
} IWICColorTransformVtbl;

interface IWICColorTransform {
    CONST_VTBL struct IWICColorTransformVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICColorTransform_QueryInterface(This,riid,ppvObject)    ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICColorTransform_AddRef(This)   ((This)->lpVtbl->AddRef(This))
#define IWICColorTransform_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICColorTransform_GetSize(This,puiWidth,puiHeight)   ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICColorTransform_GetPixelFormat(This,pPixelFormat)  ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICColorTransform_GetResolution(This,pDpiX,pDpiY)    ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICColorTransform_CopyPalette(This,pIPalette)    ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICColorTransform_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)    ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICColorTransform_Initialize(This,pIBitmapSource,pIContextSource,pIContextDest,pixelFmtDest)  ((This)->lpVtbl->Initialize(This,pIBitmapSource,pIContextSource,pIContextDest,pixelFmtDest))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICColorTransform_INTERFACE_DEFINED__ */


#ifndef __IWICFastMetadataEncoder_INTERFACE_DEFINED__
#define __IWICFastMetadataEncoder_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICFastMetadataEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("B84E2C09-78C9-4AC4-8BD3-524AE1663A2F")IWICFastMetadataEncoder:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Commit(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryWriter(IWICMetadataQueryWriter ** ppIMetadataQueryWriter) = 0;
};
#else /* C style interface */
typedef struct IWICFastMetadataEncoderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICFastMetadataEncoder * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICFastMetadataEncoder * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICFastMetadataEncoder * This);
    HRESULT(STDMETHODCALLTYPE *Commit)(IWICFastMetadataEncoder * This);
    HRESULT(STDMETHODCALLTYPE *GetMetadataQueryWriter)(IWICFastMetadataEncoder * This, IWICMetadataQueryWriter ** ppIMetadataQueryWriter);
    END_INTERFACE
} IWICFastMetadataEncoderVtbl;

interface IWICFastMetadataEncoder {
    CONST_VTBL struct IWICFastMetadataEncoderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICFastMetadataEncoder_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICFastMetadataEncoder_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICFastMetadataEncoder_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICFastMetadataEncoder_Commit(This)  ((This)->lpVtbl->Commit(This))
#define IWICFastMetadataEncoder_GetMetadataQueryWriter(This,ppIMetadataQueryWriter)   ((This)->lpVtbl->GetMetadataQueryWriter(This,ppIMetadataQueryWriter))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICFastMetadataEncoder_INTERFACE_DEFINED__ */


#ifndef __IWICStream_INTERFACE_DEFINED__
#define __IWICStream_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("135FF860-22B7-4ddf-B0F6-218F4F299A43")IWICStream:public IStream
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InitializeFromIStream(IStream * pIStream) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeFromFilename(LPCWSTR wzFileName, DWORD dwDesiredAccess) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeFromMemory(BYTE * pbBuffer, DWORD cbBufferSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeFromIStreamRegion(IStream * pIStream, ULARGE_INTEGER ulOffset, ULARGE_INTEGER ulMaxSize) = 0;
};
#else /* C style interface */
typedef struct IWICStreamVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICStream * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICStream * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICStream * This);
    HRESULT(STDMETHODCALLTYPE *Read)(IWICStream * This, void *pv, ULONG cb, ULONG * pcbRead);
    HRESULT(STDMETHODCALLTYPE *Write)(IWICStream * This, const void *pv, ULONG cb, ULONG * pcbWritten);
    HRESULT(STDMETHODCALLTYPE *Seek)(IWICStream * This, LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * plibNewPosition);
    HRESULT(STDMETHODCALLTYPE *SetSize)(IWICStream * This, ULARGE_INTEGER libNewSize);
    HRESULT(STDMETHODCALLTYPE *CopyTo)(IWICStream * This, IStream * pstm, ULARGE_INTEGER cb, ULARGE_INTEGER * pcbRead, ULARGE_INTEGER * pcbWritten);
    HRESULT(STDMETHODCALLTYPE *Commit)(IWICStream * This, DWORD grfCommitFlags);
    HRESULT(STDMETHODCALLTYPE *Revert)(IWICStream * This);
    HRESULT(STDMETHODCALLTYPE *LockRegion)(IWICStream * This, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    HRESULT(STDMETHODCALLTYPE *UnlockRegion)(IWICStream * This, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    HRESULT(STDMETHODCALLTYPE *Stat)(IWICStream * This, STATSTG * pstatstg, DWORD grfStatFlag);
    HRESULT(STDMETHODCALLTYPE *Clone)(IWICStream * This, IStream ** ppstm);
    HRESULT(STDMETHODCALLTYPE *InitializeFromIStream)(IWICStream * This, IStream * pIStream);
    HRESULT(STDMETHODCALLTYPE *InitializeFromFilename)(IWICStream * This, LPCWSTR wzFileName, DWORD dwDesiredAccess);
    HRESULT(STDMETHODCALLTYPE *InitializeFromMemory)(IWICStream * This, BYTE * pbBuffer, DWORD cbBufferSize);
    HRESULT(STDMETHODCALLTYPE *InitializeFromIStreamRegion)(IWICStream * This, IStream * pIStream, ULARGE_INTEGER ulOffset, ULARGE_INTEGER ulMaxSize);
    END_INTERFACE
} IWICStreamVtbl;

interface IWICStream {
    CONST_VTBL struct IWICStreamVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICStream_QueryInterface(This,riid,ppvObject)    ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICStream_AddRef(This)   ((This)->lpVtbl->AddRef(This))
#define IWICStream_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICStream_Read(This,pv,cb,pcbRead)   ((This)->lpVtbl->Read(This,pv,cb,pcbRead))
#define IWICStream_Write(This,pv,cb,pcbWritten)   ((This)->lpVtbl->Write(This,pv,cb,pcbWritten))
#define IWICStream_Seek(This,dlibMove,dwOrigin,plibNewPosition)   ((This)->lpVtbl->Seek(This,dlibMove,dwOrigin,plibNewPosition))
#define IWICStream_SetSize(This,libNewSize)   ((This)->lpVtbl->SetSize(This,libNewSize))
#define IWICStream_CopyTo(This,pstm,cb,pcbRead,pcbWritten)    ((This)->lpVtbl->CopyTo(This,pstm,cb,pcbRead,pcbWritten))
#define IWICStream_Commit(This,grfCommitFlags)    ((This)->lpVtbl->Commit(This,grfCommitFlags))
#define IWICStream_Revert(This)   ((This)->lpVtbl->Revert(This))
#define IWICStream_LockRegion(This,libOffset,cb,dwLockType)   ((This)->lpVtbl->LockRegion(This,libOffset,cb,dwLockType))
#define IWICStream_UnlockRegion(This,libOffset,cb,dwLockType)  ((This)->lpVtbl->UnlockRegion(This,libOffset,cb,dwLockType))
#define IWICStream_Stat(This,pstatstg,grfStatFlag)    ((This)->lpVtbl->Stat(This,pstatstg,grfStatFlag))
#define IWICStream_Clone(This,ppstm)  ((This)->lpVtbl->Clone(This,ppstm))
#define IWICStream_InitializeFromIStream(This,pIStream)   ((This)->lpVtbl->InitializeFromIStream(This,pIStream))
#define IWICStream_InitializeFromFilename(This,wzFileName,dwDesiredAccess)    ((This)->lpVtbl->InitializeFromFilename(This,wzFileName,dwDesiredAccess))
#define IWICStream_InitializeFromMemory(This,pbBuffer,cbBufferSize)   ((This)->lpVtbl->InitializeFromMemory(This,pbBuffer,cbBufferSize))
#define IWICStream_InitializeFromIStreamRegion(This,pIStream,ulOffset,ulMaxSize)  ((This)->lpVtbl->InitializeFromIStreamRegion(This,pIStream,ulOffset,ulMaxSize))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICStream_INTERFACE_DEFINED__ */

#ifndef __IWICEnumMetadataItem_INTERFACE_DEFINED__
#define __IWICEnumMetadataItem_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICEnumMetadataItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DC2BB46D-3F07-481E-8625-220C4AEDBB33")IWICEnumMetadataItem:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, PROPVARIANT * rgeltSchema, PROPVARIANT * rgeltId, PROPVARIANT * rgeltValue, ULONG * pceltFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IWICEnumMetadataItem ** ppIEnumMetadataItem) = 0;
};
#else /* C style interface */
typedef struct IWICEnumMetadataItemVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICEnumMetadataItem * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICEnumMetadataItem * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICEnumMetadataItem * This);
    HRESULT(STDMETHODCALLTYPE *Next)(IWICEnumMetadataItem * This, ULONG celt, PROPVARIANT * rgeltSchema, PROPVARIANT * rgeltId, PROPVARIANT * rgeltValue, ULONG * pceltFetched);
    HRESULT(STDMETHODCALLTYPE *Skip)(IWICEnumMetadataItem * This, ULONG celt);
    HRESULT(STDMETHODCALLTYPE *Reset)(IWICEnumMetadataItem * This);
    HRESULT(STDMETHODCALLTYPE *Clone)(IWICEnumMetadataItem * This, IWICEnumMetadataItem ** ppIEnumMetadataItem);
    END_INTERFACE
} IWICEnumMetadataItemVtbl;

interface IWICEnumMetadataItem {
    CONST_VTBL struct IWICEnumMetadataItemVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICEnumMetadataItem_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICEnumMetadataItem_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICEnumMetadataItem_Release(This)    ((This)->lpVtbl->Release(This))
#define IWICEnumMetadataItem_Next(This,celt,rgeltSchema,rgeltId,rgeltValue,pceltFetched)  ((This)->lpVtbl->Next(This,celt,rgeltSchema,rgeltId,rgeltValue,pceltFetched))
#define IWICEnumMetadataItem_Skip(This,celt)  ((This)->lpVtbl->Skip(This,celt))
#define IWICEnumMetadataItem_Reset(This)  ((This)->lpVtbl->Reset(This))
#define IWICEnumMetadataItem_Clone(This,ppIEnumMetadataItem)  ((This)->lpVtbl->Clone(This,ppIEnumMetadataItem))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICEnumMetadataItem_INTERFACE_DEFINED__ */


#ifndef __IWICMetadataQueryReader_INTERFACE_DEFINED__
#define __IWICMetadataQueryReader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICMetadataQueryReader;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("30989668-E1C9-4597-B395-458EEDB808DF")IWICMetadataQueryReader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetContainerFormat(GUID * pguidContainerFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLocation(UINT cchMaxLength, WCHAR * wzNamespace, UINT * pcchActualLength) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMetadataByName(LPCWSTR wzName, PROPVARIANT * pvarValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEnumerator(IEnumString ** ppIEnumString) = 0;
};
#else /* C style interface */
typedef struct IWICMetadataQueryReaderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICMetadataQueryReader * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICMetadataQueryReader * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICMetadataQueryReader * This);
    HRESULT(STDMETHODCALLTYPE *GetContainerFormat)(IWICMetadataQueryReader * This, GUID * pguidContainerFormat);
    HRESULT(STDMETHODCALLTYPE *GetLocation)(IWICMetadataQueryReader * This, UINT cchMaxLength, WCHAR * wzNamespace, UINT * pcchActualLength);
    HRESULT(STDMETHODCALLTYPE *GetMetadataByName)(IWICMetadataQueryReader * This, LPCWSTR wzName, PROPVARIANT * pvarValue);
    HRESULT(STDMETHODCALLTYPE *GetEnumerator)(IWICMetadataQueryReader * This, IEnumString ** ppIEnumString);
    END_INTERFACE
} IWICMetadataQueryReaderVtbl;

interface IWICMetadataQueryReader {
    CONST_VTBL struct IWICMetadataQueryReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICMetadataQueryReader_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICMetadataQueryReader_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICMetadataQueryReader_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICMetadataQueryReader_GetContainerFormat(This,pguidContainerFormat)  ((This)->lpVtbl->GetContainerFormat(This,pguidContainerFormat))
#define IWICMetadataQueryReader_GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength)   ((This)->lpVtbl->GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength))
#define IWICMetadataQueryReader_GetMetadataByName(This,wzName,pvarValue)  ((This)->lpVtbl->GetMetadataByName(This,wzName,pvarValue))
#define IWICMetadataQueryReader_GetEnumerator(This,ppIEnumString)  ((This)->lpVtbl->GetEnumerator(This,ppIEnumString))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICMetadataQueryReader_INTERFACE_DEFINED__ */

#ifndef __IWICMetadataQueryWriter_INTERFACE_DEFINED__
#define __IWICMetadataQueryWriter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICMetadataQueryWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A721791A-0DEF-4d06-BD91-2118BF1DB10B")IWICMetadataQueryWriter:public IWICMetadataQueryReader
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetMetadataByName(LPCWSTR wzName, const PROPVARIANT * pvarValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveMetadataByName(LPCWSTR wzName) = 0;
};
#else /* C style interface */
typedef struct IWICMetadataQueryWriterVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICMetadataQueryWriter * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICMetadataQueryWriter * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICMetadataQueryWriter * This);
    HRESULT(STDMETHODCALLTYPE *GetContainerFormat)(IWICMetadataQueryWriter * This, GUID * pguidContainerFormat);
    HRESULT(STDMETHODCALLTYPE *GetLocation)(IWICMetadataQueryWriter * This, UINT cchMaxLength, WCHAR * wzNamespace, UINT * pcchActualLength);
    HRESULT(STDMETHODCALLTYPE *GetMetadataByName)(IWICMetadataQueryWriter * This, LPCWSTR wzName, PROPVARIANT * pvarValue);
    HRESULT(STDMETHODCALLTYPE *GetEnumerator)(IWICMetadataQueryWriter * This, IEnumString ** ppIEnumString);
    HRESULT(STDMETHODCALLTYPE *SetMetadataByName)(IWICMetadataQueryWriter * This, LPCWSTR wzName, const PROPVARIANT * pvarValue);
    HRESULT(STDMETHODCALLTYPE *RemoveMetadataByName)(IWICMetadataQueryWriter * This, LPCWSTR wzName);
    END_INTERFACE
} IWICMetadataQueryWriterVtbl;

interface IWICMetadataQueryWriter {
    CONST_VTBL struct IWICMetadataQueryWriterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICMetadataQueryWriter_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICMetadataQueryWriter_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICMetadataQueryWriter_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICMetadataQueryWriter_GetContainerFormat(This,pguidContainerFormat)  ((This)->lpVtbl->GetContainerFormat(This,pguidContainerFormat))
#define IWICMetadataQueryWriter_GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength)   ((This)->lpVtbl->GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength))
#define IWICMetadataQueryWriter_GetMetadataByName(This,wzName,pvarValue)  ((This)->lpVtbl->GetMetadataByName(This,wzName,pvarValue))
#define IWICMetadataQueryWriter_GetEnumerator(This,ppIEnumString)  ((This)->lpVtbl->GetEnumerator(This,ppIEnumString))
#define IWICMetadataQueryWriter_SetMetadataByName(This,wzName,pvarValue)  ((This)->lpVtbl->SetMetadataByName(This,wzName,pvarValue))
#define IWICMetadataQueryWriter_RemoveMetadataByName(This,wzName)  ((This)->lpVtbl->RemoveMetadataByName(This,wzName))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICMetadataQueryWriter_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapEncoder_INTERFACE_DEFINED__
#define __IWICBitmapEncoder_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000103-a8f2-4877-ba0a-fd2b6645fb94")IWICBitmapEncoder:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IStream * pIStream, WICBitmapEncoderCacheOption cacheOption) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContainerFormat(GUID * pguidContainerFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEncoderInfo(IWICBitmapEncoderInfo ** ppIEncoderInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetColorContexts(UINT cCount, IWICColorContext ** ppIColorContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPalette(IWICPalette * pIPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetThumbnail(IWICBitmapSource * pIThumbnail) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPreview(IWICBitmapSource * pIPreview) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateNewFrame(IWICBitmapFrameEncode ** ppIFrameEncode, IPropertyBag2 ** ppIEncoderOptions) = 0;
    virtual HRESULT STDMETHODCALLTYPE Commit(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryWriter(IWICMetadataQueryWriter ** ppIMetadataQueryWriter) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapEncoderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapEncoder * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapEncoder * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapEncoder * This);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICBitmapEncoder * This, IStream * pIStream, WICBitmapEncoderCacheOption cacheOption);
    HRESULT(STDMETHODCALLTYPE *GetContainerFormat)(IWICBitmapEncoder * This, GUID * pguidContainerFormat);
    HRESULT(STDMETHODCALLTYPE *GetEncoderInfo)(IWICBitmapEncoder * This, IWICBitmapEncoderInfo ** ppIEncoderInfo);
    HRESULT(STDMETHODCALLTYPE *SetColorContexts)(IWICBitmapEncoder * This, UINT cCount, IWICColorContext ** ppIColorContext);
    HRESULT(STDMETHODCALLTYPE *SetPalette)(IWICBitmapEncoder * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *SetThumbnail)(IWICBitmapEncoder * This, IWICBitmapSource * pIThumbnail);
    HRESULT(STDMETHODCALLTYPE *SetPreview)(IWICBitmapEncoder * This, IWICBitmapSource * pIPreview);
    HRESULT(STDMETHODCALLTYPE *CreateNewFrame)(IWICBitmapEncoder * This, IWICBitmapFrameEncode ** ppIFrameEncode, IPropertyBag2 ** ppIEncoderOptions);
    HRESULT(STDMETHODCALLTYPE *Commit)(IWICBitmapEncoder * This);
    HRESULT(STDMETHODCALLTYPE *GetMetadataQueryWriter)(IWICBitmapEncoder * This, IWICMetadataQueryWriter ** ppIMetadataQueryWriter);
    END_INTERFACE
} IWICBitmapEncoderVtbl;

interface IWICBitmapEncoder {
    CONST_VTBL struct IWICBitmapEncoderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapEncoder_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapEncoder_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapEncoder_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapEncoder_Initialize(This,pIStream,cacheOption)   ((This)->lpVtbl->Initialize(This,pIStream,cacheOption))
#define IWICBitmapEncoder_GetContainerFormat(This,pguidContainerFormat)   ((This)->lpVtbl->GetContainerFormat(This,pguidContainerFormat))
#define IWICBitmapEncoder_GetEncoderInfo(This,ppIEncoderInfo)  ((This)->lpVtbl->GetEncoderInfo(This,ppIEncoderInfo))
#define IWICBitmapEncoder_SetColorContexts(This,cCount,ppIColorContext)   ((This)->lpVtbl->SetColorContexts(This,cCount,ppIColorContext))
#define IWICBitmapEncoder_SetPalette(This,pIPalette)  ((This)->lpVtbl->SetPalette(This,pIPalette))
#define IWICBitmapEncoder_SetThumbnail(This,pIThumbnail)  ((This)->lpVtbl->SetThumbnail(This,pIThumbnail))
#define IWICBitmapEncoder_SetPreview(This,pIPreview)  ((This)->lpVtbl->SetPreview(This,pIPreview))
#define IWICBitmapEncoder_CreateNewFrame(This,ppIFrameEncode,ppIEncoderOptions)   ((This)->lpVtbl->CreateNewFrame(This,ppIFrameEncode,ppIEncoderOptions))
#define IWICBitmapEncoder_Commit(This)    ((This)->lpVtbl->Commit(This))
#define IWICBitmapEncoder_GetMetadataQueryWriter(This,ppIMetadataQueryWriter)  ((This)->lpVtbl->GetMetadataQueryWriter(This,ppIMetadataQueryWriter))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapEncoder_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapFrameEncode_INTERFACE_DEFINED__
#define __IWICBitmapFrameEncode_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapFrameEncode;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000105-a8f2-4877-ba0a-fd2b6645fb94")IWICBitmapFrameEncode:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(IPropertyBag2 * pIEncoderOptions) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSize(UINT uiWidth, UINT uiHeight) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetResolution(double dpiX, double dpiY) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPixelFormat(WICPixelFormatGUID * pPixelFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetColorContexts(UINT cCount, IWICColorContext ** ppIColorContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPalette(IWICPalette * pIPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetThumbnail(IWICBitmapSource * pIThumbnail) = 0;
    virtual HRESULT STDMETHODCALLTYPE WritePixels(UINT lineCount, UINT cbStride, UINT cbBufferSize, BYTE * pbPixels) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteSource(IWICBitmapSource * pIBitmapSource, WICRect * prc) = 0;
    virtual HRESULT STDMETHODCALLTYPE Commit(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryWriter(IWICMetadataQueryWriter ** ppIMetadataQueryWriter) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapFrameEncodeVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapFrameEncode * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapFrameEncode * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapFrameEncode * This);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICBitmapFrameEncode * This, IPropertyBag2 * pIEncoderOptions);
    HRESULT(STDMETHODCALLTYPE *SetSize)(IWICBitmapFrameEncode * This, UINT uiWidth, UINT uiHeight);
    HRESULT(STDMETHODCALLTYPE *SetResolution)(IWICBitmapFrameEncode * This, double dpiX, double dpiY);
    HRESULT(STDMETHODCALLTYPE *SetPixelFormat)(IWICBitmapFrameEncode * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *SetColorContexts)(IWICBitmapFrameEncode * This, UINT cCount, IWICColorContext ** ppIColorContext);
    HRESULT(STDMETHODCALLTYPE *SetPalette)(IWICBitmapFrameEncode * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *SetThumbnail)(IWICBitmapFrameEncode * This, IWICBitmapSource * pIThumbnail);
    HRESULT(STDMETHODCALLTYPE *WritePixels)(IWICBitmapFrameEncode * This, UINT lineCount, UINT cbStride, UINT cbBufferSize, BYTE * pbPixels);
    HRESULT(STDMETHODCALLTYPE *WriteSource)(IWICBitmapFrameEncode * This, IWICBitmapSource * pIBitmapSource, WICRect * prc);
    HRESULT(STDMETHODCALLTYPE *Commit)(IWICBitmapFrameEncode * This);
    HRESULT(STDMETHODCALLTYPE *GetMetadataQueryWriter)(IWICBitmapFrameEncode * This, IWICMetadataQueryWriter ** ppIMetadataQueryWriter);
    END_INTERFACE
} IWICBitmapFrameEncodeVtbl;

interface IWICBitmapFrameEncode {
    CONST_VTBL struct IWICBitmapFrameEncodeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapFrameEncode_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapFrameEncode_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapFrameEncode_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapFrameEncode_Initialize(This,pIEncoderOptions)   ((This)->lpVtbl->Initialize(This,pIEncoderOptions))
#define IWICBitmapFrameEncode_SetSize(This,uiWidth,uiHeight)  ((This)->lpVtbl->SetSize(This,uiWidth,uiHeight))
#define IWICBitmapFrameEncode_SetResolution(This,dpiX,dpiY)   ((This)->lpVtbl->SetResolution(This,dpiX,dpiY))
#define IWICBitmapFrameEncode_SetPixelFormat(This,pPixelFormat)   ((This)->lpVtbl->SetPixelFormat(This,pPixelFormat))
#define IWICBitmapFrameEncode_SetColorContexts(This,cCount,ppIColorContext)   ((This)->lpVtbl->SetColorContexts(This,cCount,ppIColorContext))
#define IWICBitmapFrameEncode_SetPalette(This,pIPalette)  ((This)->lpVtbl->SetPalette(This,pIPalette))
#define IWICBitmapFrameEncode_SetThumbnail(This,pIThumbnail)  ((This)->lpVtbl->SetThumbnail(This,pIThumbnail))
#define IWICBitmapFrameEncode_WritePixels(This,lineCount,cbStride,cbBufferSize,pbPixels)  ((This)->lpVtbl->WritePixels(This,lineCount,cbStride,cbBufferSize,pbPixels))
#define IWICBitmapFrameEncode_WriteSource(This,pIBitmapSource,prc)    ((This)->lpVtbl->WriteSource(This,pIBitmapSource,prc))
#define IWICBitmapFrameEncode_Commit(This)    ((This)->lpVtbl->Commit(This))
#define IWICBitmapFrameEncode_GetMetadataQueryWriter(This,ppIMetadataQueryWriter)  ((This)->lpVtbl->GetMetadataQueryWriter(This,ppIMetadataQueryWriter))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapFrameEncode_INTERFACE_DEFINED__ */

#ifndef __IWICPlanarBitmapFrameEncode_INTERFACE_DEFINED__
#define __IWICPlanarBitmapFrameEncode_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICPlanarBitmapFrameEncode;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F928B7B8-2221-40C1-B72E-7E82F1974D1A")IWICPlanarBitmapFrameEncode:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE WritePixels(UINT, WICBitmapPlane *, UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteSource(IWICBitmapSource **, UINT, WICRect *) = 0;
};
#else /* C style interface */
typedef struct IWICPlanarBitmapFrameEncodeVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICPlanarBitmapFrameEncode *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICPlanarBitmapFrameEncode *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICPlanarBitmapFrameEncode *);
    HRESULT (STDMETHODCALLTYPE *WritePixels)(IWICPlanarBitmapFrameEncode *, UINT, WICBitmapPlane *, UINT);
    HRESULT (STDMETHODCALLTYPE *WriteSource)(IWICPlanarBitmapFrameEncode *, IWICBitmapSource **, UINT, WICRect *);
    END_INTERFACE
} IWICPlanarBitmapFrameEncodeVtbl;

interface IWICPlanarBitmapFrameEncode {
    CONST_VTBL struct IWICPlanarBitmapFrameEncodeVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IWICPlanarBitmapFrameEncode_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICPlanarBitmapFrameEncode_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICPlanarBitmapFrameEncode_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICPlanarBitmapFrameEncode_WritePixels(This,lineCount,pPlanes,cPlanes)  ((This)->lpVtbl->WritePixels(This,lineCount,pPlanes,cPlanes))
#define IWICPlanarBitmapFrameEncode_WriteSource(This,ppPlanes,cPlanes,prcSource)  ((This)->lpVtbl->WriteSource(This,ppPlanes,cPlanes,prcSource))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICPlanarBitmapFrameEncode_INTERFACE_DEFINED__ */

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)

extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0019_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0019_v0_0_s_ifspec;

#ifndef __IWICImageEncoder_INTERFACE_DEFINED__
#define __IWICImageEncoder_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICImageEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("04C75BF8-3CE1-473B-ACC5-3CC4F5E94999")IWICImageEncoder:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE WriteFrame(ID2D1Image *, IWICBitmapFrameEncode *, const WICImageParameters *) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteFrameThumbnail(ID2D1Image *, IWICBitmapFrameEncode *, const WICImageParameters *) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteThumbnail(ID2D1Image *, IWICBitmapEncoder *, const WICImageParameters *) = 0;
};
#else /* C style interface */
typedef struct IWICImageEncoderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICImageEncoder *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICImageEncoder *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICImageEncoder *);
    HRESULT (STDMETHODCALLTYPE *WriteFrame)(IWICImageEncoder *, ID2D1Image *, IWICBitmapFrameEncode *, const WICImageParameters *);
    HRESULT (STDMETHODCALLTYPE *WriteFrameThumbnail)(IWICImageEncoder *, ID2D1Image *, IWICBitmapFrameEncode *, const WICImageParameters *);
    HRESULT (STDMETHODCALLTYPE *WriteThumbnail)(IWICImageEncoder *, ID2D1Image *, IWICBitmapEncoder *, const WICImageParameters *);
    END_INTERFACE
} IWICImageEncoderVtbl;

interface IWICImageEncoder {
    CONST_VTBL struct IWICImageEncoderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICImageEncoder_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICImageEncoder_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICImageEncoder_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICImageEncoder_WriteFrame(This,pImage,pFrameEncode,pImageParameters)  ((This)->lpVtbl->WriteFrame(This,pImage,pFrameEncode,pImageParameters))
#define IWICImageEncoder_WriteFrameThumbnail(This,pImage,pFrameEncode,pImageParameters)  ((This)->lpVtbl->WriteFrameThumbnail(This,pImage,pFrameEncode,pImageParameters))
#define IWICImageEncoder_WriteThumbnail(This,pImage,pEncoder,pImageParameters)  ((This)->lpVtbl->WriteThumbnail(This,pImage,pEncoder,pImageParameters))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICImageEncoder_INTERFACE_DEFINED__ */

#endif

extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0020_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0020_v0_0_s_ifspec;

#ifndef __IWICBitmapDecoder_INTERFACE_DEFINED__
#define __IWICBitmapDecoder_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapDecoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9EDDE9E7-8DEE-47ea-99DF-E6FAF2ED44BF")IWICBitmapDecoder:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryCapability(IStream * pIStream, DWORD * pdwCapability) = 0;
    virtual HRESULT STDMETHODCALLTYPE Initialize(IStream * pIStream, WICDecodeOptions cacheOptions) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContainerFormat(GUID * pguidContainerFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDecoderInfo(IWICBitmapDecoderInfo ** ppIDecoderInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyPalette(IWICPalette * pIPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryReader(IWICMetadataQueryReader ** ppIMetadataQueryReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreview(IWICBitmapSource ** ppIBitmapSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColorContexts(UINT cCount, IWICColorContext ** ppIColorContexts, UINT * pcActualCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetThumbnail(IWICBitmapSource ** ppIThumbnail) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFrameCount(UINT * pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFrame(UINT index, IWICBitmapFrameDecode ** ppIBitmapFrame) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapDecoderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapDecoder * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapDecoder * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapDecoder * This);
    HRESULT(STDMETHODCALLTYPE *QueryCapability)(IWICBitmapDecoder * This, IStream * pIStream, DWORD * pdwCapability);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IWICBitmapDecoder * This, IStream * pIStream, WICDecodeOptions cacheOptions);
    HRESULT(STDMETHODCALLTYPE *GetContainerFormat)(IWICBitmapDecoder * This, GUID * pguidContainerFormat);
    HRESULT(STDMETHODCALLTYPE *GetDecoderInfo)(IWICBitmapDecoder * This, IWICBitmapDecoderInfo ** ppIDecoderInfo);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICBitmapDecoder * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *GetMetadataQueryReader)(IWICBitmapDecoder * This, IWICMetadataQueryReader ** ppIMetadataQueryReader);
    HRESULT(STDMETHODCALLTYPE *GetPreview)(IWICBitmapDecoder * This, IWICBitmapSource ** ppIBitmapSource);
    HRESULT(STDMETHODCALLTYPE *GetColorContexts)(IWICBitmapDecoder * This, UINT cCount, IWICColorContext ** ppIColorContexts, UINT * pcActualCount);
    HRESULT(STDMETHODCALLTYPE *GetThumbnail)(IWICBitmapDecoder * This, IWICBitmapSource ** ppIThumbnail);
    HRESULT(STDMETHODCALLTYPE *GetFrameCount)(IWICBitmapDecoder * This, UINT * pCount);
    HRESULT(STDMETHODCALLTYPE *GetFrame)(IWICBitmapDecoder * This, UINT index, IWICBitmapFrameDecode ** ppIBitmapFrame);
    END_INTERFACE
} IWICBitmapDecoderVtbl;

interface IWICBitmapDecoder {
    CONST_VTBL struct IWICBitmapDecoderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapDecoder_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapDecoder_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapDecoder_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapDecoder_QueryCapability(This,pIStream,pdwCapability)    ((This)->lpVtbl->QueryCapability(This,pIStream,pdwCapability))
#define IWICBitmapDecoder_Initialize(This,pIStream,cacheOptions)  ((This)->lpVtbl->Initialize(This,pIStream,cacheOptions))
#define IWICBitmapDecoder_GetContainerFormat(This,pguidContainerFormat)   ((This)->lpVtbl->GetContainerFormat(This,pguidContainerFormat))
#define IWICBitmapDecoder_GetDecoderInfo(This,ppIDecoderInfo)  ((This)->lpVtbl->GetDecoderInfo(This,ppIDecoderInfo))
#define IWICBitmapDecoder_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICBitmapDecoder_GetMetadataQueryReader(This,ppIMetadataQueryReader)  ((This)->lpVtbl->GetMetadataQueryReader(This,ppIMetadataQueryReader))
#define IWICBitmapDecoder_GetPreview(This,ppIBitmapSource)    ((This)->lpVtbl->GetPreview(This,ppIBitmapSource))
#define IWICBitmapDecoder_GetColorContexts(This,cCount,ppIColorContexts,pcActualCount)    ((This)->lpVtbl->GetColorContexts(This,cCount,ppIColorContexts,pcActualCount))
#define IWICBitmapDecoder_GetThumbnail(This,ppIThumbnail)  ((This)->lpVtbl->GetThumbnail(This,ppIThumbnail))
#define IWICBitmapDecoder_GetFrameCount(This,pCount)  ((This)->lpVtbl->GetFrameCount(This,pCount))
#define IWICBitmapDecoder_GetFrame(This,index,ppIBitmapFrame)  ((This)->lpVtbl->GetFrame(This,index,ppIBitmapFrame))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapDecoder_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapSourceTransform_INTERFACE_DEFINED__
#define __IWICBitmapSourceTransform_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapSourceTransform;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3B16811B-6A43-4ec9-B713-3D5A0C13B940")IWICBitmapSourceTransform:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CopyPixels(const WICRect * prcSrc, UINT uiWidth, UINT uiHeight, WICPixelFormatGUID * pguidDstFormat, WICBitmapTransformOptions dstTransform, UINT nStride, UINT cbBufferSize, BYTE * pbBuffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClosestSize(UINT * puiWidth, UINT * puiHeight) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClosestPixelFormat(WICPixelFormatGUID * pguidDstFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoesSupportTransform(WICBitmapTransformOptions dstTransform, BOOL * pfIsSupported) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapSourceTransformVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapSourceTransform * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapSourceTransform * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapSourceTransform * This);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICBitmapSourceTransform * This, const WICRect * prcSrc, UINT uiWidth, UINT uiHeight, WICPixelFormatGUID * pguidDstFormat, WICBitmapTransformOptions dstTransform, UINT nStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *GetClosestSize)(IWICBitmapSourceTransform * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetClosestPixelFormat)(IWICBitmapSourceTransform * This, WICPixelFormatGUID * pguidDstFormat);
    HRESULT(STDMETHODCALLTYPE *DoesSupportTransform)(IWICBitmapSourceTransform * This, WICBitmapTransformOptions dstTransform, BOOL * pfIsSupported);
    END_INTERFACE
} IWICBitmapSourceTransformVtbl;

interface IWICBitmapSourceTransform {
    CONST_VTBL struct IWICBitmapSourceTransformVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapSourceTransform_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapSourceTransform_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapSourceTransform_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapSourceTransform_CopyPixels(This,prcSrc,uiWidth,uiHeight,pguidDstFormat,dstTransform,nStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prcSrc,uiWidth,uiHeight,pguidDstFormat,dstTransform,nStride,cbBufferSize,pbBuffer))
#define IWICBitmapSourceTransform_GetClosestSize(This,puiWidth,puiHeight)  ((This)->lpVtbl->GetClosestSize(This,puiWidth,puiHeight))
#define IWICBitmapSourceTransform_GetClosestPixelFormat(This,pguidDstFormat)  ((This)->lpVtbl->GetClosestPixelFormat(This,pguidDstFormat))
#define IWICBitmapSourceTransform_DoesSupportTransform(This,dstTransform,pfIsSupported)   ((This)->lpVtbl->DoesSupportTransform(This,dstTransform,pfIsSupported))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapSourceTransform_INTERFACE_DEFINED__ */

#ifndef __IWICPlanarBitmapSourceTransform_INTERFACE_DEFINED__
#define __IWICPlanarBitmapSourceTransform_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICPlanarBitmapSourceTransform;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3AFF9CCE-BE95-4303-B927-E7D16FF4A613")IWICPlanarBitmapSourceTransform:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE DoesSupportTransform(UINT *, UINT *, WICBitmapTransformOptions, WICPlanarOptions, const WICPixelFormatGUID *, WICBitmapPlaneDescription *, UINT, BOOL *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyPixels(const WICRect *, UINT, UINT, WICBitmapTransformOptions, WICPlanarOptions, const WICBitmapPlane *, UINT) = 0;
};
#else /* C style interface */
typedef struct IWICPlanarBitmapSourceTransformVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICPlanarBitmapSourceTransform *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICPlanarBitmapSourceTransform *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICPlanarBitmapSourceTransform *);
    HRESULT (STDMETHODCALLTYPE *DoesSupportTransform)(IWICPlanarBitmapSourceTransform *, UINT *, UINT *, WICBitmapTransformOptions, WICPlanarOptions, const WICPixelFormatGUID *, WICBitmapPlaneDescription *, UINT, BOOL *);
    HRESULT (STDMETHODCALLTYPE *CopyPixels)(IWICPlanarBitmapSourceTransform *, const WICRect *, UINT, UINT, WICBitmapTransformOptions, WICPlanarOptions, const WICBitmapPlane *, UINT);
    END_INTERFACE
} IWICPlanarBitmapSourceTransformVtbl;

interface IWICPlanarBitmapSourceTransform {
    CONST_VTBL struct IWICPlanarBitmapSourceTransformVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICPlanarBitmapSourceTransform_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICPlanarBitmapSourceTransform_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICPlanarBitmapSourceTransform_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICPlanarBitmapSourceTransform_DoesSupportTransform(This,puiWidth,puiHeight,dstTransform,dstPlanarOptions,pguidDstFormats,pPlaneDescriptions,cPlanes,pfIsSupported)  ((This)->lpVtbl->DoesSupportTransform(This,puiWidth,puiHeight,dstTransform,dstPlanarOptions,pguidDstFormats,pPlaneDescriptions,cPlanes,pfIsSupported))
#define IWICPlanarBitmapSourceTransform_CopyPixels(This,prcSource,uiWidth,uiHeight,dstTransform,dstPlanarOptions,pDstPlanes,cPlanes)  ((This)->lpVtbl->CopyPixels(This,prcSource,uiWidth,uiHeight,dstTransform,dstPlanarOptions,pDstPlanes,cPlanes))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICPlanarBitmapSourceTransform_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapFrameDecode_INTERFACE_DEFINED__
#define __IWICBitmapFrameDecode_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapFrameDecode;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3B16811B-6A43-4ec9-A813-3D930C13B940")IWICBitmapFrameDecode:public IWICBitmapSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryReader(IWICMetadataQueryReader ** ppIMetadataQueryReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColorContexts(UINT cCount, IWICColorContext ** ppIColorContexts, UINT * pcActualCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetThumbnail(IWICBitmapSource ** ppIThumbnail) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapFrameDecodeVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapFrameDecode * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapFrameDecode * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapFrameDecode * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICBitmapFrameDecode * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICBitmapFrameDecode * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICBitmapFrameDecode * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICBitmapFrameDecode * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICBitmapFrameDecode * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *GetMetadataQueryReader)(IWICBitmapFrameDecode * This, IWICMetadataQueryReader ** ppIMetadataQueryReader);
    HRESULT(STDMETHODCALLTYPE *GetColorContexts)(IWICBitmapFrameDecode * This, UINT cCount, IWICColorContext ** ppIColorContexts, UINT * pcActualCount);
    HRESULT(STDMETHODCALLTYPE *GetThumbnail)(IWICBitmapFrameDecode * This, IWICBitmapSource ** ppIThumbnail);
    END_INTERFACE
} IWICBitmapFrameDecodeVtbl;

interface IWICBitmapFrameDecode {
    CONST_VTBL struct IWICBitmapFrameDecodeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapFrameDecode_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapFrameDecode_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapFrameDecode_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapFrameDecode_GetSize(This,puiWidth,puiHeight)    ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICBitmapFrameDecode_GetPixelFormat(This,pPixelFormat)   ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICBitmapFrameDecode_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICBitmapFrameDecode_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICBitmapFrameDecode_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICBitmapFrameDecode_GetMetadataQueryReader(This,ppIMetadataQueryReader)  ((This)->lpVtbl->GetMetadataQueryReader(This,ppIMetadataQueryReader))
#define IWICBitmapFrameDecode_GetColorContexts(This,cCount,ppIColorContexts,pcActualCount)    ((This)->lpVtbl->GetColorContexts(This,cCount,ppIColorContexts,pcActualCount))
#define IWICBitmapFrameDecode_GetThumbnail(This,ppIThumbnail)  ((This)->lpVtbl->GetThumbnail(This,ppIThumbnail))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapFrameDecode_INTERFACE_DEFINED__ */

#ifndef __IWICProgressiveLevelControl_INTERFACE_DEFINED__
#define __IWICProgressiveLevelControl_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICProgressiveLevelControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DAAC296F-7AA5-4dbf-8D15-225C5976F891")IWICProgressiveLevelControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetLevelCount(UINT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentLevel(UINT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCurrentLevel(UINT) = 0;
};
#else /* C style interface */
typedef struct IWICProgressiveLevelControlVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICProgressiveLevelControl *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICProgressiveLevelControl *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICProgressiveLevelControl *);
    HRESULT (STDMETHODCALLTYPE *GetLevelCount)(IWICProgressiveLevelControl *, UINT *);
    HRESULT (STDMETHODCALLTYPE *GetCurrentLevel)(IWICProgressiveLevelControl *, UINT *);
    HRESULT (STDMETHODCALLTYPE *SetCurrentLevel)(IWICProgressiveLevelControl *, UINT);
    END_INTERFACE
} IWICProgressiveLevelControlVtbl;

interface IWICProgressiveLevelControl {
    CONST_VTBL struct IWICProgressiveLevelControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICProgressiveLevelControl_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICProgressiveLevelControl_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICProgressiveLevelControl_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICProgressiveLevelControl_GetLevelCount(This,pcLevels)  ((This)->lpVtbl->GetLevelCount(This,pcLevels))
#define IWICProgressiveLevelControl_GetCurrentLevel(This,pnLevel)  ((This)->lpVtbl->GetCurrentLevel(This,pnLevel))
#define IWICProgressiveLevelControl_SetCurrentLevel(This,nLevel)  ((This)->lpVtbl->SetCurrentLevel(This,nLevel))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICProgressiveLevelControl_INTERFACE_DEFINED__ */

#ifndef __IWICProgressCallback_INTERFACE_DEFINED__
#define __IWICProgressCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICProgressCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4776F9CD-9517-45FA-BF24-E89C5EC5C60C")IWICProgressCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Notify(ULONG, WICProgressOperation, double) = 0;
};
#else /* C style interface */
typedef struct IWICProgressCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICProgressCallback *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICProgressCallback *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICProgressCallback *);
    HRESULT (STDMETHODCALLTYPE *Notify)(IWICProgressCallback *, ULONG, WICProgressOperation, double);
    END_INTERFACE
} IWICProgressCallbackVtbl;

interface IWICProgressCallback {
    CONST_VTBL struct IWICProgressCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICProgressCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICProgressCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICProgressCallback_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICProgressCallback_Notify(This,uFrameNum,operation,dblProgress)  ((This)->lpVtbl->Notify(This,uFrameNum,operation,dblProgress))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICProgressCallback_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapCodecProgressNotification_INTERFACE_DEFINED__
#define __IWICBitmapCodecProgressNotification_INTERFACE_DEFINED__

typedef HRESULT(__stdcall * PFNProgressNotification)(LPVOID pvData, ULONG uFrameNum, WICProgressOperation operation, double dblProgress);

EXTERN_C const IID IID_IWICBitmapCodecProgressNotification;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("64C1024E-C3CF-4462-8078-88C2B11C46D9")IWICBitmapCodecProgressNotification:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterProgressNotification(PFNProgressNotification pfnProgressNotification, LPVOID pvData, DWORD dwProgressFlags) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapCodecProgressNotificationVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapCodecProgressNotification * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapCodecProgressNotification * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapCodecProgressNotification * This);
    HRESULT(STDMETHODCALLTYPE *RegisterProgressNotification)(IWICBitmapCodecProgressNotification * This, PFNProgressNotification pfnProgressNotification, LPVOID pvData, DWORD dwProgressFlags);
    END_INTERFACE
} IWICBitmapCodecProgressNotificationVtbl;

interface IWICBitmapCodecProgressNotification {
    CONST_VTBL struct IWICBitmapCodecProgressNotificationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapCodecProgressNotification_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapCodecProgressNotification_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICBitmapCodecProgressNotification_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICBitmapCodecProgressNotification_RegisterProgressNotification(This,pfnProgressNotification,pvData,dwProgressFlags)  ((This)->lpVtbl->RegisterProgressNotification(This,pfnProgressNotification,pvData,dwProgressFlags))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapCodecProgressNotification_INTERFACE_DEFINED__ */

#ifndef __IWICComponentInfo_INTERFACE_DEFINED__
#define __IWICComponentInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICComponentInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("23BC3F0A-698B-4357-886B-F24D50671334")IWICComponentInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetComponentType(WICComponentType * pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCLSID(CLSID * pclsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSigningStatus(DWORD * pStatus) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAuthor(UINT cchAuthor, WCHAR * wzAuthor, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVendorGUID(GUID * pguidVendor) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVersion(UINT cchVersion, WCHAR * wzVersion, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSpecVersion(UINT cchSpecVersion, WCHAR * wzSpecVersion, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFriendlyName(UINT cchFriendlyName, WCHAR * wzFriendlyName, UINT * pcchActual) = 0;
};
#else /* C style interface */
typedef struct IWICComponentInfoVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICComponentInfo * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICComponentInfo * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICComponentInfo * This);
    HRESULT(STDMETHODCALLTYPE *GetComponentType)(IWICComponentInfo * This, WICComponentType * pType);
    HRESULT(STDMETHODCALLTYPE *GetCLSID)(IWICComponentInfo * This, CLSID * pclsid);
    HRESULT(STDMETHODCALLTYPE *GetSigningStatus)(IWICComponentInfo * This, DWORD * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetAuthor)(IWICComponentInfo * This, UINT cchAuthor, WCHAR * wzAuthor, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetVendorGUID)(IWICComponentInfo * This, GUID * pguidVendor);
    HRESULT(STDMETHODCALLTYPE *GetVersion)(IWICComponentInfo * This, UINT cchVersion, WCHAR * wzVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetSpecVersion)(IWICComponentInfo * This, UINT cchSpecVersion, WCHAR * wzSpecVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFriendlyName)(IWICComponentInfo * This, UINT cchFriendlyName, WCHAR * wzFriendlyName, UINT * pcchActual);
    END_INTERFACE
} IWICComponentInfoVtbl;

interface IWICComponentInfo {
    CONST_VTBL struct IWICComponentInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICComponentInfo_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICComponentInfo_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICComponentInfo_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICComponentInfo_GetComponentType(This,pType)    ((This)->lpVtbl->GetComponentType(This,pType))
#define IWICComponentInfo_GetCLSID(This,pclsid)   ((This)->lpVtbl->GetCLSID(This,pclsid))
#define IWICComponentInfo_GetSigningStatus(This,pStatus)  ((This)->lpVtbl->GetSigningStatus(This,pStatus))
#define IWICComponentInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)   ((This)->lpVtbl->GetAuthor(This,cchAuthor,wzAuthor,pcchActual))
#define IWICComponentInfo_GetVendorGUID(This,pguidVendor)  ((This)->lpVtbl->GetVendorGUID(This,pguidVendor))
#define IWICComponentInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)    ((This)->lpVtbl->GetVersion(This,cchVersion,wzVersion,pcchActual))
#define IWICComponentInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)    ((This)->lpVtbl->GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual))
#define IWICComponentInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)  ((This)->lpVtbl->GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICComponentInfo_INTERFACE_DEFINED__ */

#ifndef __IWICFormatConverterInfo_INTERFACE_DEFINED__
#define __IWICFormatConverterInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICFormatConverterInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9F34FB65-13F4-4f15-BC57-3726B5E53D9F")IWICFormatConverterInfo:public IWICComponentInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetPixelFormats(UINT cFormats, WICPixelFormatGUID * pPixelFormatGUIDs, UINT * pcActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(IWICFormatConverter ** ppIConverter) = 0;
};
#else /* C style interface */
typedef struct IWICFormatConverterInfoVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICFormatConverterInfo * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICFormatConverterInfo * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICFormatConverterInfo * This);
    HRESULT(STDMETHODCALLTYPE *GetComponentType)(IWICFormatConverterInfo * This, WICComponentType * pType);
    HRESULT(STDMETHODCALLTYPE *GetCLSID)(IWICFormatConverterInfo * This, CLSID * pclsid);
    HRESULT(STDMETHODCALLTYPE *GetSigningStatus)(IWICFormatConverterInfo * This, DWORD * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetAuthor)(IWICFormatConverterInfo * This, UINT cchAuthor, WCHAR * wzAuthor, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetVendorGUID)(IWICFormatConverterInfo * This, GUID * pguidVendor);
    HRESULT(STDMETHODCALLTYPE *GetVersion)(IWICFormatConverterInfo * This, UINT cchVersion, WCHAR * wzVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetSpecVersion)(IWICFormatConverterInfo * This, UINT cchSpecVersion, WCHAR * wzSpecVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFriendlyName)(IWICFormatConverterInfo * This, UINT cchFriendlyName, WCHAR * wzFriendlyName, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormats)(IWICFormatConverterInfo * This, UINT cFormats, WICPixelFormatGUID * pPixelFormatGUIDs, UINT * pcActual);
    HRESULT(STDMETHODCALLTYPE *CreateInstance)(IWICFormatConverterInfo * This, IWICFormatConverter ** ppIConverter);
    END_INTERFACE
} IWICFormatConverterInfoVtbl;

interface IWICFormatConverterInfo {
    CONST_VTBL struct IWICFormatConverterInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICFormatConverterInfo_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICFormatConverterInfo_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICFormatConverterInfo_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICFormatConverterInfo_GetComponentType(This,pType)  ((This)->lpVtbl->GetComponentType(This,pType))
#define IWICFormatConverterInfo_GetCLSID(This,pclsid)  ((This)->lpVtbl->GetCLSID(This,pclsid))
#define IWICFormatConverterInfo_GetSigningStatus(This,pStatus)    ((This)->lpVtbl->GetSigningStatus(This,pStatus))
#define IWICFormatConverterInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)  ((This)->lpVtbl->GetAuthor(This,cchAuthor,wzAuthor,pcchActual))
#define IWICFormatConverterInfo_GetVendorGUID(This,pguidVendor)   ((This)->lpVtbl->GetVendorGUID(This,pguidVendor))
#define IWICFormatConverterInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)  ((This)->lpVtbl->GetVersion(This,cchVersion,wzVersion,pcchActual))
#define IWICFormatConverterInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)  ((This)->lpVtbl->GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual))
#define IWICFormatConverterInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)   ((This)->lpVtbl->GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual))
#define IWICFormatConverterInfo_GetPixelFormats(This,cFormats,pPixelFormatGUIDs,pcActual)  ((This)->lpVtbl->GetPixelFormats(This,cFormats,pPixelFormatGUIDs,pcActual))
#define IWICFormatConverterInfo_CreateInstance(This,ppIConverter)  ((This)->lpVtbl->CreateInstance(This,ppIConverter))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICFormatConverterInfo_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapCodecInfo_INTERFACE_DEFINED__
#define __IWICBitmapCodecInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapCodecInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("E87A44C4-B76E-4c47-8B09-298EB12A2714")IWICBitmapCodecInfo:public IWICComponentInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetContainerFormat(GUID * pguidContainerFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPixelFormats(UINT cFormats, GUID * pguidPixelFormats, UINT * pcActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColorManagementVersion(UINT cchColorManagementVersion, WCHAR * wzColorManagementVersion, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceManufacturer(UINT cchDeviceManufacturer, WCHAR * wzDeviceManufacturer, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceModels(UINT cchDeviceModels, WCHAR * wzDeviceModels, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMimeTypes(UINT cchMimeTypes, WCHAR * wzMimeTypes, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFileExtensions(UINT cchFileExtensions, WCHAR * wzFileExtensions, UINT * pcchActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoesSupportAnimation(BOOL * pfSupportAnimation) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoesSupportChromakey(BOOL * pfSupportChromakey) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoesSupportLossless(BOOL * pfSupportLossless) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoesSupportMultiframe(BOOL * pfSupportMultiframe) = 0;
    virtual HRESULT STDMETHODCALLTYPE MatchesMimeType(LPCWSTR wzMimeType, BOOL * pfMatches) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapCodecInfoVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapCodecInfo * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapCodecInfo * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapCodecInfo * This);
    HRESULT(STDMETHODCALLTYPE *GetComponentType)(IWICBitmapCodecInfo * This, WICComponentType * pType);
    HRESULT(STDMETHODCALLTYPE *GetCLSID)(IWICBitmapCodecInfo * This, CLSID * pclsid);
    HRESULT(STDMETHODCALLTYPE *GetSigningStatus)(IWICBitmapCodecInfo * This, DWORD * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetAuthor)(IWICBitmapCodecInfo * This, UINT cchAuthor, WCHAR * wzAuthor, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetVendorGUID)(IWICBitmapCodecInfo * This, GUID * pguidVendor);
    HRESULT(STDMETHODCALLTYPE *GetVersion)(IWICBitmapCodecInfo * This, UINT cchVersion, WCHAR * wzVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetSpecVersion)(IWICBitmapCodecInfo * This, UINT cchSpecVersion, WCHAR * wzSpecVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFriendlyName)(IWICBitmapCodecInfo * This, UINT cchFriendlyName, WCHAR * wzFriendlyName, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetContainerFormat)(IWICBitmapCodecInfo * This, GUID * pguidContainerFormat);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormats)(IWICBitmapCodecInfo * This, UINT cFormats, GUID * pguidPixelFormats, UINT * pcActual);
    HRESULT(STDMETHODCALLTYPE *GetColorManagementVersion)(IWICBitmapCodecInfo * This, UINT cchColorManagementVersion, WCHAR * wzColorManagementVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetDeviceManufacturer)(IWICBitmapCodecInfo * This, UINT cchDeviceManufacturer, WCHAR * wzDeviceManufacturer, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetDeviceModels)(IWICBitmapCodecInfo * This, UINT cchDeviceModels, WCHAR * wzDeviceModels, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetMimeTypes)(IWICBitmapCodecInfo * This, UINT cchMimeTypes, WCHAR * wzMimeTypes, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFileExtensions)(IWICBitmapCodecInfo * This, UINT cchFileExtensions, WCHAR * wzFileExtensions, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *DoesSupportAnimation)(IWICBitmapCodecInfo * This, BOOL * pfSupportAnimation);
    HRESULT(STDMETHODCALLTYPE *DoesSupportChromakey)(IWICBitmapCodecInfo * This, BOOL * pfSupportChromakey);
    HRESULT(STDMETHODCALLTYPE *DoesSupportLossless)(IWICBitmapCodecInfo * This, BOOL * pfSupportLossless);
    HRESULT(STDMETHODCALLTYPE *DoesSupportMultiframe)(IWICBitmapCodecInfo * This, BOOL * pfSupportMultiframe);
    HRESULT(STDMETHODCALLTYPE *MatchesMimeType)(IWICBitmapCodecInfo * This, LPCWSTR wzMimeType, BOOL * pfMatches);
    END_INTERFACE
} IWICBitmapCodecInfoVtbl;

interface IWICBitmapCodecInfo {
    CONST_VTBL struct IWICBitmapCodecInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapCodecInfo_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapCodecInfo_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICBitmapCodecInfo_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICBitmapCodecInfo_GetComponentType(This,pType)  ((This)->lpVtbl->GetComponentType(This,pType))
#define IWICBitmapCodecInfo_GetCLSID(This,pclsid)  ((This)->lpVtbl->GetCLSID(This,pclsid))
#define IWICBitmapCodecInfo_GetSigningStatus(This,pStatus)    ((This)->lpVtbl->GetSigningStatus(This,pStatus))
#define IWICBitmapCodecInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)  ((This)->lpVtbl->GetAuthor(This,cchAuthor,wzAuthor,pcchActual))
#define IWICBitmapCodecInfo_GetVendorGUID(This,pguidVendor)   ((This)->lpVtbl->GetVendorGUID(This,pguidVendor))
#define IWICBitmapCodecInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)  ((This)->lpVtbl->GetVersion(This,cchVersion,wzVersion,pcchActual))
#define IWICBitmapCodecInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)  ((This)->lpVtbl->GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual))
#define IWICBitmapCodecInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)   ((This)->lpVtbl->GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual))
#define IWICBitmapCodecInfo_GetContainerFormat(This,pguidContainerFormat)  ((This)->lpVtbl->GetContainerFormat(This,pguidContainerFormat))
#define IWICBitmapCodecInfo_GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual)  ((This)->lpVtbl->GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual))
#define IWICBitmapCodecInfo_GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual)  ((This)->lpVtbl->GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual))
#define IWICBitmapCodecInfo_GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual)  ((This)->lpVtbl->GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual))
#define IWICBitmapCodecInfo_GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual)   ((This)->lpVtbl->GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual))
#define IWICBitmapCodecInfo_GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual)    ((This)->lpVtbl->GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual))
#define IWICBitmapCodecInfo_GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual)  ((This)->lpVtbl->GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual))
#define IWICBitmapCodecInfo_DoesSupportAnimation(This,pfSupportAnimation)  ((This)->lpVtbl->DoesSupportAnimation(This,pfSupportAnimation))
#define IWICBitmapCodecInfo_DoesSupportChromakey(This,pfSupportChromakey)  ((This)->lpVtbl->DoesSupportChromakey(This,pfSupportChromakey))
#define IWICBitmapCodecInfo_DoesSupportLossless(This,pfSupportLossless)   ((This)->lpVtbl->DoesSupportLossless(This,pfSupportLossless))
#define IWICBitmapCodecInfo_DoesSupportMultiframe(This,pfSupportMultiframe)   ((This)->lpVtbl->DoesSupportMultiframe(This,pfSupportMultiframe))
#define IWICBitmapCodecInfo_MatchesMimeType(This,wzMimeType,pfMatches)    ((This)->lpVtbl->MatchesMimeType(This,wzMimeType,pfMatches))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapCodecInfo_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapEncoderInfo_INTERFACE_DEFINED__
#define __IWICBitmapEncoderInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapEncoderInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("94C9B4EE-A09F-4f92-8A1E-4A9BCE7E76FB")IWICBitmapEncoderInfo:public IWICBitmapCodecInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(IWICBitmapEncoder ** ppIBitmapEncoder) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapEncoderInfoVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapEncoderInfo * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapEncoderInfo * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapEncoderInfo * This);
    HRESULT(STDMETHODCALLTYPE *GetComponentType)(IWICBitmapEncoderInfo * This, WICComponentType * pType);
    HRESULT(STDMETHODCALLTYPE *GetCLSID)(IWICBitmapEncoderInfo * This, CLSID * pclsid);
    HRESULT(STDMETHODCALLTYPE *GetSigningStatus)(IWICBitmapEncoderInfo * This, DWORD * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetAuthor)(IWICBitmapEncoderInfo * This, UINT cchAuthor, WCHAR * wzAuthor, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetVendorGUID)(IWICBitmapEncoderInfo * This, GUID * pguidVendor);
    HRESULT(STDMETHODCALLTYPE *GetVersion)(IWICBitmapEncoderInfo * This, UINT cchVersion, WCHAR * wzVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetSpecVersion)(IWICBitmapEncoderInfo * This, UINT cchSpecVersion, WCHAR * wzSpecVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFriendlyName)(IWICBitmapEncoderInfo * This, UINT cchFriendlyName, WCHAR * wzFriendlyName, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetContainerFormat)(IWICBitmapEncoderInfo * This, GUID * pguidContainerFormat);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormats)(IWICBitmapEncoderInfo * This, UINT cFormats, GUID * pguidPixelFormats, UINT * pcActual);
    HRESULT(STDMETHODCALLTYPE *GetColorManagementVersion)(IWICBitmapEncoderInfo * This, UINT cchColorManagementVersion, WCHAR * wzColorManagementVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetDeviceManufacturer)(IWICBitmapEncoderInfo * This, UINT cchDeviceManufacturer, WCHAR * wzDeviceManufacturer, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetDeviceModels)(IWICBitmapEncoderInfo * This, UINT cchDeviceModels, WCHAR * wzDeviceModels, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetMimeTypes)(IWICBitmapEncoderInfo * This, UINT cchMimeTypes, WCHAR * wzMimeTypes, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFileExtensions)(IWICBitmapEncoderInfo * This, UINT cchFileExtensions, WCHAR * wzFileExtensions, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *DoesSupportAnimation)(IWICBitmapEncoderInfo * This, BOOL * pfSupportAnimation);
    HRESULT(STDMETHODCALLTYPE *DoesSupportChromakey)(IWICBitmapEncoderInfo * This, BOOL * pfSupportChromakey);
    HRESULT(STDMETHODCALLTYPE *DoesSupportLossless)(IWICBitmapEncoderInfo * This, BOOL * pfSupportLossless);
    HRESULT(STDMETHODCALLTYPE *DoesSupportMultiframe)(IWICBitmapEncoderInfo * This, BOOL * pfSupportMultiframe);
    HRESULT(STDMETHODCALLTYPE *MatchesMimeType)(IWICBitmapEncoderInfo * This, LPCWSTR wzMimeType, BOOL * pfMatches);
    HRESULT(STDMETHODCALLTYPE *CreateInstance)(IWICBitmapEncoderInfo * This, IWICBitmapEncoder ** ppIBitmapEncoder);
    END_INTERFACE
} IWICBitmapEncoderInfoVtbl;

interface IWICBitmapEncoderInfo {
    CONST_VTBL struct IWICBitmapEncoderInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapEncoderInfo_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapEncoderInfo_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapEncoderInfo_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapEncoderInfo_GetComponentType(This,pType)    ((This)->lpVtbl->GetComponentType(This,pType))
#define IWICBitmapEncoderInfo_GetCLSID(This,pclsid)   ((This)->lpVtbl->GetCLSID(This,pclsid))
#define IWICBitmapEncoderInfo_GetSigningStatus(This,pStatus)  ((This)->lpVtbl->GetSigningStatus(This,pStatus))
#define IWICBitmapEncoderInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)   ((This)->lpVtbl->GetAuthor(This,cchAuthor,wzAuthor,pcchActual))
#define IWICBitmapEncoderInfo_GetVendorGUID(This,pguidVendor)  ((This)->lpVtbl->GetVendorGUID(This,pguidVendor))
#define IWICBitmapEncoderInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)    ((This)->lpVtbl->GetVersion(This,cchVersion,wzVersion,pcchActual))
#define IWICBitmapEncoderInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)    ((This)->lpVtbl->GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual))
#define IWICBitmapEncoderInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)  ((This)->lpVtbl->GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual))
#define IWICBitmapEncoderInfo_GetContainerFormat(This,pguidContainerFormat)   ((This)->lpVtbl->GetContainerFormat(This,pguidContainerFormat))
#define IWICBitmapEncoderInfo_GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual)   ((This)->lpVtbl->GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual))
#define IWICBitmapEncoderInfo_GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual)   ((This)->lpVtbl->GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual))
#define IWICBitmapEncoderInfo_GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual)   ((This)->lpVtbl->GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual))
#define IWICBitmapEncoderInfo_GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual)  ((This)->lpVtbl->GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual))
#define IWICBitmapEncoderInfo_GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual)  ((This)->lpVtbl->GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual))
#define IWICBitmapEncoderInfo_GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual)   ((This)->lpVtbl->GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual))
#define IWICBitmapEncoderInfo_DoesSupportAnimation(This,pfSupportAnimation)   ((This)->lpVtbl->DoesSupportAnimation(This,pfSupportAnimation))
#define IWICBitmapEncoderInfo_DoesSupportChromakey(This,pfSupportChromakey)   ((This)->lpVtbl->DoesSupportChromakey(This,pfSupportChromakey))
#define IWICBitmapEncoderInfo_DoesSupportLossless(This,pfSupportLossless)  ((This)->lpVtbl->DoesSupportLossless(This,pfSupportLossless))
#define IWICBitmapEncoderInfo_DoesSupportMultiframe(This,pfSupportMultiframe)  ((This)->lpVtbl->DoesSupportMultiframe(This,pfSupportMultiframe))
#define IWICBitmapEncoderInfo_MatchesMimeType(This,wzMimeType,pfMatches)  ((This)->lpVtbl->MatchesMimeType(This,wzMimeType,pfMatches))
#define IWICBitmapEncoderInfo_CreateInstance(This,ppIBitmapEncoder)   ((This)->lpVtbl->CreateInstance(This,ppIBitmapEncoder))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapEncoderInfo_INTERFACE_DEFINED__ */

#ifndef __IWICBitmapDecoderInfo_INTERFACE_DEFINED__
#define __IWICBitmapDecoderInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICBitmapDecoderInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D8CD007F-D08F-4191-9BFC-236EA7F0E4B5")IWICBitmapDecoderInfo:public IWICBitmapCodecInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetPatterns(UINT cbSizePatterns, WICBitmapPattern * pPatterns, UINT * pcPatterns, UINT * pcbPatternsActual) = 0;
    virtual HRESULT STDMETHODCALLTYPE MatchesPattern(IStream * pIStream, BOOL * pfMatches) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(IWICBitmapDecoder ** ppIBitmapDecoder) = 0;
};
#else /* C style interface */
typedef struct IWICBitmapDecoderInfoVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICBitmapDecoderInfo * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICBitmapDecoderInfo * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICBitmapDecoderInfo * This);
    HRESULT(STDMETHODCALLTYPE *GetComponentType)(IWICBitmapDecoderInfo * This, WICComponentType * pType);
    HRESULT(STDMETHODCALLTYPE *GetCLSID)(IWICBitmapDecoderInfo * This, CLSID * pclsid);
    HRESULT(STDMETHODCALLTYPE *GetSigningStatus)(IWICBitmapDecoderInfo * This, DWORD * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetAuthor)(IWICBitmapDecoderInfo * This, UINT cchAuthor, WCHAR * wzAuthor, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetVendorGUID)(IWICBitmapDecoderInfo * This, GUID * pguidVendor);
    HRESULT(STDMETHODCALLTYPE *GetVersion)(IWICBitmapDecoderInfo * This, UINT cchVersion, WCHAR * wzVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetSpecVersion)(IWICBitmapDecoderInfo * This, UINT cchSpecVersion, WCHAR * wzSpecVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFriendlyName)(IWICBitmapDecoderInfo * This, UINT cchFriendlyName, WCHAR * wzFriendlyName, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetContainerFormat)(IWICBitmapDecoderInfo * This, GUID * pguidContainerFormat);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormats)(IWICBitmapDecoderInfo * This, UINT cFormats, GUID * pguidPixelFormats, UINT * pcActual);
    HRESULT(STDMETHODCALLTYPE *GetColorManagementVersion)(IWICBitmapDecoderInfo * This, UINT cchColorManagementVersion, WCHAR * wzColorManagementVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetDeviceManufacturer)(IWICBitmapDecoderInfo * This, UINT cchDeviceManufacturer, WCHAR * wzDeviceManufacturer, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetDeviceModels)(IWICBitmapDecoderInfo * This, UINT cchDeviceModels, WCHAR * wzDeviceModels, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetMimeTypes)(IWICBitmapDecoderInfo * This, UINT cchMimeTypes, WCHAR * wzMimeTypes, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFileExtensions)(IWICBitmapDecoderInfo * This, UINT cchFileExtensions, WCHAR * wzFileExtensions, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *DoesSupportAnimation)(IWICBitmapDecoderInfo * This, BOOL * pfSupportAnimation);
    HRESULT(STDMETHODCALLTYPE *DoesSupportChromakey)(IWICBitmapDecoderInfo * This, BOOL * pfSupportChromakey);
    HRESULT(STDMETHODCALLTYPE *DoesSupportLossless)(IWICBitmapDecoderInfo * This, BOOL * pfSupportLossless);
    HRESULT(STDMETHODCALLTYPE *DoesSupportMultiframe)(IWICBitmapDecoderInfo * This, BOOL * pfSupportMultiframe);
    HRESULT(STDMETHODCALLTYPE *MatchesMimeType)(IWICBitmapDecoderInfo * This, LPCWSTR wzMimeType, BOOL * pfMatches);
    HRESULT(STDMETHODCALLTYPE *GetPatterns)(IWICBitmapDecoderInfo * This, UINT cbSizePatterns, WICBitmapPattern * pPatterns, UINT * pcPatterns, UINT * pcbPatternsActual);
    HRESULT(STDMETHODCALLTYPE *MatchesPattern)(IWICBitmapDecoderInfo * This, IStream * pIStream, BOOL * pfMatches);
    HRESULT(STDMETHODCALLTYPE *CreateInstance)(IWICBitmapDecoderInfo * This, IWICBitmapDecoder ** ppIBitmapDecoder);
    END_INTERFACE
} IWICBitmapDecoderInfoVtbl;

interface IWICBitmapDecoderInfo {
    CONST_VTBL struct IWICBitmapDecoderInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICBitmapDecoderInfo_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICBitmapDecoderInfo_AddRef(This)    ((This)->lpVtbl->AddRef(This))
#define IWICBitmapDecoderInfo_Release(This)   ((This)->lpVtbl->Release(This))
#define IWICBitmapDecoderInfo_GetComponentType(This,pType)    ((This)->lpVtbl->GetComponentType(This,pType))
#define IWICBitmapDecoderInfo_GetCLSID(This,pclsid)   ((This)->lpVtbl->GetCLSID(This,pclsid))
#define IWICBitmapDecoderInfo_GetSigningStatus(This,pStatus)  ((This)->lpVtbl->GetSigningStatus(This,pStatus))
#define IWICBitmapDecoderInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)   ((This)->lpVtbl->GetAuthor(This,cchAuthor,wzAuthor,pcchActual))
#define IWICBitmapDecoderInfo_GetVendorGUID(This,pguidVendor)  ((This)->lpVtbl->GetVendorGUID(This,pguidVendor))
#define IWICBitmapDecoderInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)    ((This)->lpVtbl->GetVersion(This,cchVersion,wzVersion,pcchActual))
#define IWICBitmapDecoderInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)    ((This)->lpVtbl->GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual))
#define IWICBitmapDecoderInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)  ((This)->lpVtbl->GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual))
#define IWICBitmapDecoderInfo_GetContainerFormat(This,pguidContainerFormat)   ((This)->lpVtbl->GetContainerFormat(This,pguidContainerFormat))
#define IWICBitmapDecoderInfo_GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual)   ((This)->lpVtbl->GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual))
#define IWICBitmapDecoderInfo_GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual)   ((This)->lpVtbl->GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual))
#define IWICBitmapDecoderInfo_GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual)   ((This)->lpVtbl->GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual))
#define IWICBitmapDecoderInfo_GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual)  ((This)->lpVtbl->GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual))
#define IWICBitmapDecoderInfo_GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual)  ((This)->lpVtbl->GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual))
#define IWICBitmapDecoderInfo_GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual)   ((This)->lpVtbl->GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual))
#define IWICBitmapDecoderInfo_DoesSupportAnimation(This,pfSupportAnimation)   ((This)->lpVtbl->DoesSupportAnimation(This,pfSupportAnimation))
#define IWICBitmapDecoderInfo_DoesSupportChromakey(This,pfSupportChromakey)   ((This)->lpVtbl->DoesSupportChromakey(This,pfSupportChromakey))
#define IWICBitmapDecoderInfo_DoesSupportLossless(This,pfSupportLossless)  ((This)->lpVtbl->DoesSupportLossless(This,pfSupportLossless))
#define IWICBitmapDecoderInfo_DoesSupportMultiframe(This,pfSupportMultiframe)  ((This)->lpVtbl->DoesSupportMultiframe(This,pfSupportMultiframe))
#define IWICBitmapDecoderInfo_MatchesMimeType(This,wzMimeType,pfMatches)  ((This)->lpVtbl->MatchesMimeType(This,wzMimeType,pfMatches))
#define IWICBitmapDecoderInfo_GetPatterns(This,cbSizePatterns,pPatterns,pcPatterns,pcbPatternsActual)  ((This)->lpVtbl->GetPatterns(This,cbSizePatterns,pPatterns,pcPatterns,pcbPatternsActual))
#define IWICBitmapDecoderInfo_MatchesPattern(This,pIStream,pfMatches)  ((This)->lpVtbl->MatchesPattern(This,pIStream,pfMatches))
#define IWICBitmapDecoderInfo_CreateInstance(This,ppIBitmapDecoder)   ((This)->lpVtbl->CreateInstance(This,ppIBitmapDecoder))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICBitmapDecoderInfo_INTERFACE_DEFINED__ */

#ifndef __IWICPixelFormatInfo_INTERFACE_DEFINED__
#define __IWICPixelFormatInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICPixelFormatInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("E8EDA601-3D48-431a-AB44-69059BE88BBE")IWICPixelFormatInfo:public IWICComponentInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetFormatGUID(GUID * pFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColorContext(IWICColorContext ** ppIColorContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBitsPerPixel(UINT * puiBitsPerPixel) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelCount(UINT * puiChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelMask(UINT uiChannelIndex, UINT cbMaskBuffer, BYTE * pbMaskBuffer, UINT * pcbActual) = 0;
};
#else /* C style interface */
typedef struct IWICPixelFormatInfoVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICPixelFormatInfo * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICPixelFormatInfo * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICPixelFormatInfo * This);
    HRESULT(STDMETHODCALLTYPE *GetComponentType)(IWICPixelFormatInfo * This, WICComponentType * pType);
    HRESULT(STDMETHODCALLTYPE *GetCLSID)(IWICPixelFormatInfo * This, CLSID * pclsid);
    HRESULT(STDMETHODCALLTYPE *GetSigningStatus)(IWICPixelFormatInfo * This, DWORD * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetAuthor)(IWICPixelFormatInfo * This, UINT cchAuthor, WCHAR * wzAuthor, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetVendorGUID)(IWICPixelFormatInfo * This, GUID * pguidVendor);
    HRESULT(STDMETHODCALLTYPE *GetVersion)(IWICPixelFormatInfo * This, UINT cchVersion, WCHAR * wzVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetSpecVersion)(IWICPixelFormatInfo * This, UINT cchSpecVersion, WCHAR * wzSpecVersion, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFriendlyName)(IWICPixelFormatInfo * This, UINT cchFriendlyName, WCHAR * wzFriendlyName, UINT * pcchActual);
    HRESULT(STDMETHODCALLTYPE *GetFormatGUID)(IWICPixelFormatInfo * This, GUID * pFormat);
    HRESULT(STDMETHODCALLTYPE *GetColorContext)(IWICPixelFormatInfo * This, IWICColorContext ** ppIColorContext);
    HRESULT(STDMETHODCALLTYPE *GetBitsPerPixel)(IWICPixelFormatInfo * This, UINT * puiBitsPerPixel);
    HRESULT(STDMETHODCALLTYPE *GetChannelCount)(IWICPixelFormatInfo * This, UINT * puiChannelCount);
    HRESULT(STDMETHODCALLTYPE *GetChannelMask)(IWICPixelFormatInfo * This, UINT uiChannelIndex, UINT cbMaskBuffer, BYTE * pbMaskBuffer, UINT * pcbActual);
    END_INTERFACE
} IWICPixelFormatInfoVtbl;

interface IWICPixelFormatInfo {
    CONST_VTBL struct IWICPixelFormatInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICPixelFormatInfo_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICPixelFormatInfo_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICPixelFormatInfo_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICPixelFormatInfo_GetComponentType(This,pType)  ((This)->lpVtbl->GetComponentType(This,pType))
#define IWICPixelFormatInfo_GetCLSID(This,pclsid)  ((This)->lpVtbl->GetCLSID(This,pclsid))
#define IWICPixelFormatInfo_GetSigningStatus(This,pStatus)    ((This)->lpVtbl->GetSigningStatus(This,pStatus))
#define IWICPixelFormatInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)  ((This)->lpVtbl->GetAuthor(This,cchAuthor,wzAuthor,pcchActual))
#define IWICPixelFormatInfo_GetVendorGUID(This,pguidVendor)   ((This)->lpVtbl->GetVendorGUID(This,pguidVendor))
#define IWICPixelFormatInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)  ((This)->lpVtbl->GetVersion(This,cchVersion,wzVersion,pcchActual))
#define IWICPixelFormatInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)  ((This)->lpVtbl->GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual))
#define IWICPixelFormatInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)   ((This)->lpVtbl->GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual))
#define IWICPixelFormatInfo_GetFormatGUID(This,pFormat)   ((This)->lpVtbl->GetFormatGUID(This,pFormat))
#define IWICPixelFormatInfo_GetColorContext(This,ppIColorContext)  ((This)->lpVtbl->GetColorContext(This,ppIColorContext))
#define IWICPixelFormatInfo_GetBitsPerPixel(This,puiBitsPerPixel)  ((This)->lpVtbl->GetBitsPerPixel(This,puiBitsPerPixel))
#define IWICPixelFormatInfo_GetChannelCount(This,puiChannelCount)  ((This)->lpVtbl->GetChannelCount(This,puiChannelCount))
#define IWICPixelFormatInfo_GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual)   ((This)->lpVtbl->GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICPixelFormatInfo_INTERFACE_DEFINED__ */

#ifndef __IWICPixelFormatInfo2_INTERFACE_DEFINED__
#define __IWICPixelFormatInfo2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICPixelFormatInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A9DB33A2-AF5F-43C7-B679-74F5984B5AA4")IWICPixelFormatInfo2:public IWICPixelFormatInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SupportsTransparency(BOOL *) = 0;
    virtual HRESULT STDMETHODCALLTYPE(WICPixelFormatNumericRepresentation *) = 0;
};
#else /* C style interface */
typedef struct IWICPixelFormatInfo2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWICPixelFormatInfo2 *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICPixelFormatInfo2 *);
    ULONG(STDMETHODCALLTYPE *Release)(IWICPixelFormatInfo2 *);
    HRESULT(STDMETHODCALLTYPE *GetComponentType)(IWICPixelFormatInfo2 *, WICComponentType *);
    HRESULT(STDMETHODCALLTYPE *GetCLSID)(IWICPixelFormatInfo2 *, CLSID *);
    HRESULT(STDMETHODCALLTYPE *GetSigningStatus)(IWICPixelFormatInfo2 *, DWORD *);
    HRESULT(STDMETHODCALLTYPE *GetAuthor)(IWICPixelFormatInfo2 *, UINT, WCHAR *, UINT *);
    HRESULT(STDMETHODCALLTYPE *GetVendorGUID)(IWICPixelFormatInfo2 *, GUID *);
    HRESULT(STDMETHODCALLTYPE *GetVersion)(IWICPixelFormatInfo2 *, UINT, WCHAR *, UINT *);
    HRESULT(STDMETHODCALLTYPE *GetSpecVersion)(IWICPixelFormatInfo2 *, UINT, WCHAR *, UINT *);
    HRESULT(STDMETHODCALLTYPE *GetFriendlyName)(IWICPixelFormatInfo2 *, UINT, WCHAR *, UINT *);
    HRESULT(STDMETHODCALLTYPE *GetFormatGUID)(IWICPixelFormatInfo2 *, GUID *);
    HRESULT(STDMETHODCALLTYPE *GetColorContext)(IWICPixelFormatInfo2 *, IWICColorContext **);
    HRESULT(STDMETHODCALLTYPE *GetBitsPerPixel)(IWICPixelFormatInfo2 *, UINT *);
    HRESULT(STDMETHODCALLTYPE *GetChannelCount)(IWICPixelFormatInfo2 *, UINT *);
    HRESULT(STDMETHODCALLTYPE *GetChannelMask)(IWICPixelFormatInfo2 *, UINT, UINT, BYTE *, UINT *);
    HRESULT(STDMETHODCALLTYPE *SupportsTransparency)(IWICPixelFormatInfo2 *, BOOL *);
    HRESULT(STDMETHODCALLTYPE *GetNumericRepresentation)(IWICPixelFormatInfo2 *, WICPixelFormatNumericRepresentation *);
    END_INTERFACE
} IWICPixelFormatInfo2Vtbl;

interface IWICPixelFormatInfo2 {
    CONST_VTBL struct IWICPixelFormatInfo2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICPixelFormatInfo2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICPixelFormatInfo2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICPixelFormatInfo2_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICPixelFormatInfo2_GetComponentType(This,pType)  ((This)->lpVtbl->GetComponentType(This,pType))
#define IWICPixelFormatInfo2_GetCLSID(This,pclsid)  ((This)->lpVtbl->GetCLSID(This,pclsid))
#define IWICPixelFormatInfo2_GetSigningStatus(This,pStatus)  ((This)->lpVtbl->GetSigningStatus(This,pStatus))
#define IWICPixelFormatInfo2_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)  ((This)->lpVtbl->GetAuthor(This,cchAuthor,wzAuthor,pcchActual))
#define IWICPixelFormatInfo2_GetVendorGUID(This,pguidVendor)  ((This)->lpVtbl->GetVendorGUID(This,pguidVendor))
#define IWICPixelFormatInfo2_GetVersion(This,cchVersion,wzVersion,pcchActual)  ((This)->lpVtbl->GetVersion(This,cchVersion,wzVersion,pcchActual))
#define IWICPixelFormatInfo2_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)  ((This)->lpVtbl->GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual))
#define IWICPixelFormatInfo2_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)  ((This)->lpVtbl->GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual))
#define IWICPixelFormatInfo2_GetFormatGUID(This,pFormat)  ((This)->lpVtbl->GetFormatGUID(This,pFormat))
#define IWICPixelFormatInfo2_GetColorContext(This,ppIColorContext)  ((This)->lpVtbl->GetColorContext(This,ppIColorContext))
#define IWICPixelFormatInfo2_GetBitsPerPixel(This,puiBitsPerPixel)  ((This)->lpVtbl->GetBitsPerPixel(This,puiBitsPerPixel))
#define IWICPixelFormatInfo2_GetChannelCount(This,puiChannelCount)  ((This)->lpVtbl->GetChannelCount(This,puiChannelCount))
#define IWICPixelFormatInfo2_GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual)  ((This)->lpVtbl->GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual))
#define IWICPixelFormatInfo2_SupportsTransparency(This,pfSupportsTransparency)  ((This)->lpVtbl->SupportsTransparency(This,pfSupportsTransparency))
#define IWICPixelFormatInfo2_GetNumericRepresentation(This,pNumericRepresentation)  ((This)->lpVtbl->GetNumericRepresentation(This,pNumericRepresentation))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICPixelFormatInfo2_INTERFACE_DEFINED__ */

#ifndef __IWICImagingFactory_INTERFACE_DEFINED__
#define __IWICImagingFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICImagingFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("ec5ec8a9-c395-4314-9c77-54d7a935ff70")IWICImagingFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateDecoderFromFilename(LPCWSTR wzFilename, const GUID * pguidVendor, DWORD dwDesiredAccess, WICDecodeOptions metadataOptions, IWICBitmapDecoder ** ppIDecoder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDecoderFromStream(IStream * pIStream, const GUID * pguidVendor, WICDecodeOptions metadataOptions, IWICBitmapDecoder ** ppIDecoder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDecoderFromFileHandle(ULONG_PTR hFile, const GUID * pguidVendor, WICDecodeOptions metadataOptions, IWICBitmapDecoder ** ppIDecoder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateComponentInfo(REFCLSID clsidComponent, IWICComponentInfo ** ppIInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDecoder(REFGUID guidContainerFormat, const GUID * pguidVendor, IWICBitmapDecoder ** ppIDecoder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateEncoder(REFGUID guidContainerFormat, const GUID * pguidVendor, IWICBitmapEncoder ** ppIEncoder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePalette(IWICPalette ** ppIPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFormatConverter(IWICFormatConverter ** ppIFormatConverter) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapScaler(IWICBitmapScaler ** ppIBitmapScaler) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapClipper(IWICBitmapClipper ** ppIBitmapClipper) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapFlipRotator(IWICBitmapFlipRotator ** ppIBitmapFlipRotator) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateStream(IWICStream ** ppIWICStream) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateColorContext(IWICColorContext ** ppIWICColorContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateColorTransformer(IWICColorTransform ** ppIWICColorTransform) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmap(UINT uiWidth, UINT uiHeight, REFWICPixelFormatGUID pixelFormat, WICBitmapCreateCacheOption option, IWICBitmap ** ppIBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromSource(IWICBitmapSource * pIBitmapSource, WICBitmapCreateCacheOption option, IWICBitmap ** ppIBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromSourceRect(IWICBitmapSource * pIBitmapSource, UINT x, UINT y, UINT width, UINT height, IWICBitmap ** ppIBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromMemory(UINT uiWidth, UINT uiHeight, REFWICPixelFormatGUID pixelFormat, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer, IWICBitmap ** ppIBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromHBITMAP(HBITMAP hBitmap, HPALETTE hPalette, WICBitmapAlphaChannelOption options, IWICBitmap ** ppIBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromHICON(HICON hIcon, IWICBitmap ** ppIBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateComponentEnumerator(DWORD componentTypes, DWORD options, IEnumUnknown ** ppIEnumUnknown) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFastMetadataEncoderFromDecoder(IWICBitmapDecoder * pIDecoder, IWICFastMetadataEncoder ** ppIFastEncoder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFastMetadataEncoderFromFrameDecode(IWICBitmapFrameDecode * pIFrameDecoder, IWICFastMetadataEncoder ** ppIFastEncoder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateQueryWriter(REFGUID guidMetadataFormat, const GUID * pguidVendor, IWICMetadataQueryWriter ** ppIQueryWriter) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateQueryWriterFromReader(IWICMetadataQueryReader * pIQueryReader, const GUID * pguidVendor, IWICMetadataQueryWriter ** ppIQueryWriter) = 0;
};
#else /* C style interface */
typedef struct IWICImagingFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICImagingFactory * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICImagingFactory * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICImagingFactory * This);
    HRESULT(STDMETHODCALLTYPE *CreateDecoderFromFilename)(IWICImagingFactory * This, LPCWSTR wzFilename, const GUID * pguidVendor, DWORD dwDesiredAccess, WICDecodeOptions metadataOptions, IWICBitmapDecoder ** ppIDecoder);
    HRESULT(STDMETHODCALLTYPE *CreateDecoderFromStream)(IWICImagingFactory * This, IStream * pIStream, const GUID * pguidVendor, WICDecodeOptions metadataOptions, IWICBitmapDecoder ** ppIDecoder);
    HRESULT(STDMETHODCALLTYPE *CreateDecoderFromFileHandle)(IWICImagingFactory * This, ULONG_PTR hFile, const GUID * pguidVendor, WICDecodeOptions metadataOptions, IWICBitmapDecoder ** ppIDecoder);
    HRESULT(STDMETHODCALLTYPE *CreateComponentInfo)(IWICImagingFactory * This, REFCLSID clsidComponent, IWICComponentInfo ** ppIInfo);
    HRESULT(STDMETHODCALLTYPE *CreateDecoder)(IWICImagingFactory * This, REFGUID guidContainerFormat, const GUID * pguidVendor, IWICBitmapDecoder ** ppIDecoder);
    HRESULT(STDMETHODCALLTYPE *CreateEncoder)(IWICImagingFactory * This, REFGUID guidContainerFormat, const GUID * pguidVendor, IWICBitmapEncoder ** ppIEncoder);
    HRESULT(STDMETHODCALLTYPE *CreatePalette)(IWICImagingFactory * This, IWICPalette ** ppIPalette);
    HRESULT(STDMETHODCALLTYPE *CreateFormatConverter)(IWICImagingFactory * This, IWICFormatConverter ** ppIFormatConverter);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapScaler)(IWICImagingFactory * This, IWICBitmapScaler ** ppIBitmapScaler);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapClipper)(IWICImagingFactory * This, IWICBitmapClipper ** ppIBitmapClipper);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFlipRotator)(IWICImagingFactory * This, IWICBitmapFlipRotator ** ppIBitmapFlipRotator);
    HRESULT(STDMETHODCALLTYPE *CreateStream)(IWICImagingFactory * This, IWICStream ** ppIWICStream);
    HRESULT(STDMETHODCALLTYPE *CreateColorContext)(IWICImagingFactory * This, IWICColorContext ** ppIWICColorContext);
    HRESULT(STDMETHODCALLTYPE *CreateColorTransformer)(IWICImagingFactory * This, IWICColorTransform ** ppIWICColorTransform);
    HRESULT(STDMETHODCALLTYPE *CreateBitmap)(IWICImagingFactory * This, UINT uiWidth, UINT uiHeight, REFWICPixelFormatGUID pixelFormat, WICBitmapCreateCacheOption option, IWICBitmap ** ppIBitmap);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromSource)(IWICImagingFactory * This, IWICBitmapSource * pIBitmapSource, WICBitmapCreateCacheOption option, IWICBitmap ** ppIBitmap);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromSourceRect)(IWICImagingFactory * This, IWICBitmapSource * pIBitmapSource, UINT x, UINT y, UINT width, UINT height, IWICBitmap ** ppIBitmap);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromMemory)(IWICImagingFactory * This, UINT uiWidth, UINT uiHeight, REFWICPixelFormatGUID pixelFormat, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer, IWICBitmap ** ppIBitmap);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromHBITMAP)(IWICImagingFactory * This, HBITMAP hBitmap, HPALETTE hPalette, WICBitmapAlphaChannelOption options, IWICBitmap ** ppIBitmap);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromHICON)(IWICImagingFactory * This, HICON hIcon, IWICBitmap ** ppIBitmap);
    HRESULT(STDMETHODCALLTYPE *CreateComponentEnumerator)(IWICImagingFactory * This, DWORD componentTypes, DWORD options, IEnumUnknown ** ppIEnumUnknown);
    HRESULT(STDMETHODCALLTYPE *CreateFastMetadataEncoderFromDecoder)(IWICImagingFactory * This, IWICBitmapDecoder * pIDecoder, IWICFastMetadataEncoder ** ppIFastEncoder);
    HRESULT(STDMETHODCALLTYPE *CreateFastMetadataEncoderFromFrameDecode)(IWICImagingFactory * This, IWICBitmapFrameDecode * pIFrameDecoder, IWICFastMetadataEncoder ** ppIFastEncoder);
    HRESULT(STDMETHODCALLTYPE *CreateQueryWriter)(IWICImagingFactory * This, REFGUID guidMetadataFormat, const GUID * pguidVendor, IWICMetadataQueryWriter ** ppIQueryWriter);
    HRESULT(STDMETHODCALLTYPE *CreateQueryWriterFromReader)(IWICImagingFactory * This, IWICMetadataQueryReader * pIQueryReader, const GUID * pguidVendor, IWICMetadataQueryWriter ** ppIQueryWriter);
    END_INTERFACE
} IWICImagingFactoryVtbl;

interface IWICImagingFactory {
    CONST_VTBL struct IWICImagingFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICImagingFactory_QueryInterface(This,riid,ppvObject)    ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICImagingFactory_AddRef(This)   ((This)->lpVtbl->AddRef(This))
#define IWICImagingFactory_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICImagingFactory_CreateDecoderFromFilename(This,wzFilename,pguidVendor,dwDesiredAccess,metadataOptions,ppIDecoder)  ((This)->lpVtbl->CreateDecoderFromFilename(This,wzFilename,pguidVendor,dwDesiredAccess,metadataOptions,ppIDecoder))
#define IWICImagingFactory_CreateDecoderFromStream(This,pIStream,pguidVendor,metadataOptions,ppIDecoder)  ((This)->lpVtbl->CreateDecoderFromStream(This,pIStream,pguidVendor,metadataOptions,ppIDecoder))
#define IWICImagingFactory_CreateDecoderFromFileHandle(This,hFile,pguidVendor,metadataOptions,ppIDecoder)  ((This)->lpVtbl->CreateDecoderFromFileHandle(This,hFile,pguidVendor,metadataOptions,ppIDecoder))
#define IWICImagingFactory_CreateComponentInfo(This,clsidComponent,ppIInfo)   ((This)->lpVtbl->CreateComponentInfo(This,clsidComponent,ppIInfo))
#define IWICImagingFactory_CreateDecoder(This,guidContainerFormat,pguidVendor,ppIDecoder)  ((This)->lpVtbl->CreateDecoder(This,guidContainerFormat,pguidVendor,ppIDecoder))
#define IWICImagingFactory_CreateEncoder(This,guidContainerFormat,pguidVendor,ppIEncoder)  ((This)->lpVtbl->CreateEncoder(This,guidContainerFormat,pguidVendor,ppIEncoder))
#define IWICImagingFactory_CreatePalette(This,ppIPalette)  ((This)->lpVtbl->CreatePalette(This,ppIPalette))
#define IWICImagingFactory_CreateFormatConverter(This,ppIFormatConverter)  ((This)->lpVtbl->CreateFormatConverter(This,ppIFormatConverter))
#define IWICImagingFactory_CreateBitmapScaler(This,ppIBitmapScaler)   ((This)->lpVtbl->CreateBitmapScaler(This,ppIBitmapScaler))
#define IWICImagingFactory_CreateBitmapClipper(This,ppIBitmapClipper)  ((This)->lpVtbl->CreateBitmapClipper(This,ppIBitmapClipper))
#define IWICImagingFactory_CreateBitmapFlipRotator(This,ppIBitmapFlipRotator)  ((This)->lpVtbl->CreateBitmapFlipRotator(This,ppIBitmapFlipRotator))
#define IWICImagingFactory_CreateStream(This,ppIWICStream)    ((This)->lpVtbl->CreateStream(This,ppIWICStream))
#define IWICImagingFactory_CreateColorContext(This,ppIWICColorContext)    ((This)->lpVtbl->CreateColorContext(This,ppIWICColorContext))
#define IWICImagingFactory_CreateColorTransformer(This,ppIWICColorTransform)  ((This)->lpVtbl->CreateColorTransformer(This,ppIWICColorTransform))
#define IWICImagingFactory_CreateBitmap(This,uiWidth,uiHeight,pixelFormat,option,ppIBitmap)   ((This)->lpVtbl->CreateBitmap(This,uiWidth,uiHeight,pixelFormat,option,ppIBitmap))
#define IWICImagingFactory_CreateBitmapFromSource(This,pIBitmapSource,option,ppIBitmap)   ((This)->lpVtbl->CreateBitmapFromSource(This,pIBitmapSource,option,ppIBitmap))
#define IWICImagingFactory_CreateBitmapFromSourceRect(This,pIBitmapSource,x,y,width,height,ppIBitmap)  ((This)->lpVtbl->CreateBitmapFromSourceRect(This,pIBitmapSource,x,y,width,height,ppIBitmap))
#define IWICImagingFactory_CreateBitmapFromMemory(This,uiWidth,uiHeight,pixelFormat,cbStride,cbBufferSize,pbBuffer,ppIBitmap)  ((This)->lpVtbl->CreateBitmapFromMemory(This,uiWidth,uiHeight,pixelFormat,cbStride,cbBufferSize,pbBuffer,ppIBitmap))
#define IWICImagingFactory_CreateBitmapFromHBITMAP(This,hBitmap,hPalette,options,ppIBitmap)   ((This)->lpVtbl->CreateBitmapFromHBITMAP(This,hBitmap,hPalette,options,ppIBitmap))
#define IWICImagingFactory_CreateBitmapFromHICON(This,hIcon,ppIBitmap)    ((This)->lpVtbl->CreateBitmapFromHICON(This,hIcon,ppIBitmap))
#define IWICImagingFactory_CreateComponentEnumerator(This,componentTypes,options,ppIEnumUnknown)  ((This)->lpVtbl->CreateComponentEnumerator(This,componentTypes,options,ppIEnumUnknown))
#define IWICImagingFactory_CreateFastMetadataEncoderFromDecoder(This,pIDecoder,ppIFastEncoder)    ((This)->lpVtbl->CreateFastMetadataEncoderFromDecoder(This,pIDecoder,ppIFastEncoder))
#define IWICImagingFactory_CreateFastMetadataEncoderFromFrameDecode(This,pIFrameDecoder,ppIFastEncoder)   ((This)->lpVtbl->CreateFastMetadataEncoderFromFrameDecode(This,pIFrameDecoder,ppIFastEncoder))
#define IWICImagingFactory_CreateQueryWriter(This,guidMetadataFormat,pguidVendor,ppIQueryWriter)  ((This)->lpVtbl->CreateQueryWriter(This,guidMetadataFormat,pguidVendor,ppIQueryWriter))
#define IWICImagingFactory_CreateQueryWriterFromReader(This,pIQueryReader,pguidVendor,ppIQueryWriter)  ((This)->lpVtbl->CreateQueryWriterFromReader(This,pIQueryReader,pguidVendor,ppIQueryWriter))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICImagingFactory_INTERFACE_DEFINED__ */

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)

extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0035_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0035_v0_0_s_ifspec;

#ifndef __IWICImagingFactory2_INTERFACE_DEFINED__
#define __IWICImagingFactory2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICImagingFactory2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7B816B45-1996-4476-B132-DE9E247C8AF0")IWICImagingFactory2:public IWICImagingFactory
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateImageEncoder(ID2D1Device *, IWICImageEncoder **) = 0;
};
#else /* C style interface */
typedef struct IWICImagingFactory2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWICImagingFactory2 *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICImagingFactory2 *);
    ULONG(STDMETHODCALLTYPE *Release)(IWICImagingFactory2 *);
    HRESULT(STDMETHODCALLTYPE *CreateDecoderFromFilename)(IWICImagingFactory2 *, LPCWSTR, const GUID *, DWORD, WICDecodeOptions, IWICBitmapDecoder **);
    HRESULT(STDMETHODCALLTYPE *CreateDecoderFromStream)(IWICImagingFactory2 *, IStream *, const GUID *, WICDecodeOptions, IWICBitmapDecoder **);
    HRESULT(STDMETHODCALLTYPE *CreateDecoderFromFileHandle)(IWICImagingFactory2 *, ULONG_PTR, const GUID *, WICDecodeOptions, IWICBitmapDecoder **);
    HRESULT(STDMETHODCALLTYPE *CreateComponentInfo)(IWICImagingFactory2 *, REFCLSID, IWICComponentInfo **);
    HRESULT(STDMETHODCALLTYPE *CreateDecoder)(IWICImagingFactory2 *, REFGUID, const GUID *, IWICBitmapDecoder **);
    HRESULT(STDMETHODCALLTYPE *CreateEncoder)(IWICImagingFactory2 *, REFGUID, const GUID *, IWICBitmapEncoder **);
    HRESULT(STDMETHODCALLTYPE *CreatePalette)(IWICImagingFactory2 *, IWICPalette **);
    HRESULT(STDMETHODCALLTYPE *CreateFormatConverter)(IWICImagingFactory2 *, IWICFormatConverter **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapScaler)(IWICImagingFactory2 *, IWICBitmapScaler **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapClipper)(IWICImagingFactory2 *, IWICBitmapClipper **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFlipRotator)(IWICImagingFactory2 *, IWICBitmapFlipRotator **);
    HRESULT(STDMETHODCALLTYPE *CreateStream)(IWICImagingFactory2 *, IWICStream **);
    HRESULT(STDMETHODCALLTYPE *CreateColorContext)(IWICImagingFactory2 *, IWICColorContext **);
    HRESULT(STDMETHODCALLTYPE *CreateColorTransformer)(IWICImagingFactory2 *, IWICColorTransform **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmap)(IWICImagingFactory2 *, UINT, UINT, REFWICPixelFormatGUID, WICBitmapCreateCacheOption, IWICBitmap **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromSource)(IWICImagingFactory2 *, IWICBitmapSource *, WICBitmapCreateCacheOption, IWICBitmap **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromSourceRect)(IWICImagingFactory2 *, IWICBitmapSource *, UINT, UINT, UINT, UINT, IWICBitmap **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromMemory)(IWICImagingFactory2 *, UINT, UINT, REFWICPixelFormatGUID, UINT, UINT, BYTE *, IWICBitmap **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromHBITMAP)(IWICImagingFactory2 *, HBITMAP, HPALETTE, WICBitmapAlphaChannelOption, IWICBitmap **);
    HRESULT(STDMETHODCALLTYPE *CreateBitmapFromHICON)(IWICImagingFactory2 *, HICON, IWICBitmap **);
    HRESULT(STDMETHODCALLTYPE *CreateComponentEnumerator)(IWICImagingFactory2 *, DWORD, DWORD, IEnumUnknown **);
    HRESULT(STDMETHODCALLTYPE *CreateFastMetadataEncoderFromDecoder)(IWICImagingFactory2 *, IWICBitmapDecoder *, IWICFastMetadataEncoder **);
    HRESULT(STDMETHODCALLTYPE *CreateFastMetadataEncoderFromFrameDecode)(IWICImagingFactory2 *, IWICBitmapFrameDecode *, IWICFastMetadataEncoder **);
    HRESULT(STDMETHODCALLTYPE *CreateQueryWriter)(IWICImagingFactory2 *, REFGUID, const GUID *, IWICMetadataQueryWriter **);
    HRESULT(STDMETHODCALLTYPE *CreateQueryWriterFromReader)(IWICImagingFactory2 *, IWICMetadataQueryReader *, const GUID *, IWICMetadataQueryWriter **);
    HRESULT(STDMETHODCALLTYPE *CreateImageEncoder)(IWICImagingFactory2 *, ID2D1Device *, IWICImageEncoder **);
    END_INTERFACE
} IWICImagingFactory2Vtbl;

interface IWICImagingFactory2 {
    CONST_VTBL struct IWICImagingFactory2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICImagingFactory2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IWICImagingFactory2_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IWICImagingFactory2_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IWICImagingFactory2_CreateDecoderFromFilename(This,wzFilename,pguidVendor,dwDesiredAccess,metadataOptions,ppIDecoder)  ((This)->lpVtbl->CreateDecoderFromFilename(This,wzFilename,pguidVendor,dwDesiredAccess,metadataOptions,ppIDecoder)) 
#define IWICImagingFactory2_CreateDecoderFromStream(This,pIStream,pguidVendor,metadataOptions,ppIDecoder)  ((This)->lpVtbl->CreateDecoderFromStream(This,pIStream,pguidVendor,metadataOptions,ppIDecoder)) 
#define IWICImagingFactory2_CreateDecoderFromFileHandle(This,hFile,pguidVendor,metadataOptions,ppIDecoder)  ((This)->lpVtbl->CreateDecoderFromFileHandle(This,hFile,pguidVendor,metadataOptions,ppIDecoder)) 
#define IWICImagingFactory2_CreateComponentInfo(This,clsidComponent,ppIInfo)  ((This)->lpVtbl->CreateComponentInfo(This,clsidComponent,ppIInfo)) 
#define IWICImagingFactory2_CreateDecoder(This,guidContainerFormat,pguidVendor,ppIDecoder)  ((This)->lpVtbl->CreateDecoder(This,guidContainerFormat,pguidVendor,ppIDecoder)) 
#define IWICImagingFactory2_CreateEncoder(This,guidContainerFormat,pguidVendor,ppIEncoder)  ((This)->lpVtbl->CreateEncoder(This,guidContainerFormat,pguidVendor,ppIEncoder)) 
#define IWICImagingFactory2_CreatePalette(This,ppIPalette)  ((This)->lpVtbl->CreatePalette(This,ppIPalette)) 
#define IWICImagingFactory2_CreateFormatConverter(This,ppIFormatConverter)  ((This)->lpVtbl->CreateFormatConverter(This,ppIFormatConverter)) 
#define IWICImagingFactory2_CreateBitmapScaler(This,ppIBitmapScaler)  ((This)->lpVtbl->CreateBitmapScaler(This,ppIBitmapScaler)) 
#define IWICImagingFactory2_CreateBitmapClipper(This,ppIBitmapClipper)  ((This)->lpVtbl->CreateBitmapClipper(This,ppIBitmapClipper)) 
#define IWICImagingFactory2_CreateBitmapFlipRotator(This,ppIBitmapFlipRotator)  ((This)->lpVtbl->CreateBitmapFlipRotator(This,ppIBitmapFlipRotator)) 
#define IWICImagingFactory2_CreateStream(This,ppIWICStream)  ((This)->lpVtbl->CreateStream(This,ppIWICStream)) 
#define IWICImagingFactory2_CreateColorContext(This,ppIWICColorContext)  ((This)->lpVtbl->CreateColorContext(This,ppIWICColorContext)) 
#define IWICImagingFactory2_CreateColorTransformer(This,ppIWICColorTransform)  ((This)->lpVtbl->CreateColorTransformer(This,ppIWICColorTransform)) 
#define IWICImagingFactory2_CreateBitmap(This,uiWidth,uiHeight,pixelFormat,option,ppIBitmap)  ((This)->lpVtbl->CreateBitmap(This,uiWidth,uiHeight,pixelFormat,option,ppIBitmap)) 
#define IWICImagingFactory2_CreateBitmapFromSource(This,pIBitmapSource,option,ppIBitmap)  ((This)->lpVtbl->CreateBitmapFromSource(This,pIBitmapSource,option,ppIBitmap)) 
#define IWICImagingFactory2_CreateBitmapFromSourceRect(This,pIBitmapSource,x,y,width,height,ppIBitmap)  ((This)->lpVtbl->CreateBitmapFromSourceRect(This,pIBitmapSource,x,y,width,height,ppIBitmap)) 
#define IWICImagingFactory2_CreateBitmapFromMemory(This,uiWidth,uiHeight,pixelFormat,cbStride,cbBufferSize,pbBuffer,ppIBitmap)  ((This)->lpVtbl->CreateBitmapFromMemory(This,uiWidth,uiHeight,pixelFormat,cbStride,cbBufferSize,pbBuffer,ppIBitmap)) 
#define IWICImagingFactory2_CreateBitmapFromHBITMAP(This,hBitmap,hPalette,options,ppIBitmap)  ((This)->lpVtbl->CreateBitmapFromHBITMAP(This,hBitmap,hPalette,options,ppIBitmap)) 
#define IWICImagingFactory2_CreateBitmapFromHICON(This,hIcon,ppIBitmap)  ((This)->lpVtbl->CreateBitmapFromHICON(This,hIcon,ppIBitmap)) 
#define IWICImagingFactory2_CreateComponentEnumerator(This,componentTypes,options,ppIEnumUnknown)  ((This)->lpVtbl->CreateComponentEnumerator(This,componentTypes,options,ppIEnumUnknown)) 
#define IWICImagingFactory2_CreateFastMetadataEncoderFromDecoder(This,pIDecoder,ppIFastEncoder)  ((This)->lpVtbl->CreateFastMetadataEncoderFromDecoder(This,pIDecoder,ppIFastEncoder)) 
#define IWICImagingFactory2_CreateFastMetadataEncoderFromFrameDecode(This,pIFrameDecoder,ppIFastEncoder)  ((This)->lpVtbl->CreateFastMetadataEncoderFromFrameDecode(This,pIFrameDecoder,ppIFastEncoder)) 
#define IWICImagingFactory2_CreateQueryWriter(This,guidMetadataFormat,pguidVendor,ppIQueryWriter)  ((This)->lpVtbl->CreateQueryWriter(This,guidMetadataFormat,pguidVendor,ppIQueryWriter)) 
#define IWICImagingFactory2_CreateQueryWriterFromReader(This,pIQueryReader,pguidVendor,ppIQueryWriter)  ((This)->lpVtbl->CreateQueryWriterFromReader(This,pIQueryReader,pguidVendor,ppIQueryWriter)) 
#define IWICImagingFactory2_CreateImageEncoder(This,pD2DDevice,ppWICImageEncoder)  ((This)->lpVtbl->CreateImageEncoder(This,pD2DDevice,ppWICImageEncoder)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICImagingFactory2_INTERFACE_DEFINED__ */

#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */

HRESULT WINAPI WICConvertBitmapSource(REFWICPixelFormatGUID, IWICBitmapSource *, IWICBitmapSource **);
HRESULT WINAPI WICCreateBitmapFromSection(UINT, UINT, REFWICPixelFormatGUID, HANDLE, UINT, UINT, IWICBitmap **);
HRESULT WINAPI WICCreateBitmapFromSectionEx(UINT, UINT, REFWICPixelFormatGUID, HANDLE, UINT, UINT, WICSectionAccessLevel, IWICBitmap **);
HRESULT WINAPI WICMapGuidToShortName(REFGUID, UINT, WCHAR *, UINT *);
HRESULT WINAPI WICMapShortNameToGuid(const WCHAR *, GUID *);
HRESULT WINAPI WICMapSchemaToName(REFGUID, LPWSTR, UINT, WCHAR *, UINT *);

#define FACILITY_WINCODEC_ERR  0x898
#define WINCODEC_ERR_BASE  0x2000
#define MAKE_WINCODECHR(sev,code)  MAKE_HRESULT(sev,FACILITY_WINCODEC_ERR,(WINCODEC_ERR_BASE+code))
#define MAKE_WINCODECHR_ERR(code)  MAKE_WINCODECHR(1,code)
#define WINCODEC_ERR_GENERIC_ERROR    E_FAIL
#define WINCODEC_ERR_INVALIDPARAMETER  E_INVALIDARG
#define WINCODEC_ERR_OUTOFMEMORY  E_OUTOFMEMORY
#define WINCODEC_ERR_NOTIMPLEMENTED   E_NOTIMPL
#define WINCODEC_ERR_ABORTED  E_ABORT
#define WINCODEC_ERR_ACCESSDENIED  E_ACCESSDENIED
#define WINCODEC_ERR_VALUEOVERFLOW    INTSAFE_E_ARITHMETIC_OVERFLOW

/*
#define WINCODEC_ERR_WRONGSTATE   MAKE_WINCODECHR_ERR(0xf04)
#define WINCODEC_ERR_VALUEOUTOFRANGE  MAKE_WINCODECHR_ERR(0xf05)
#define WINCODEC_ERR_UNKNOWNIMAGEFORMAT   MAKE_WINCODECHR_ERR(0xf07)
#define WINCODEC_ERR_UNSUPPORTEDVERSION   MAKE_WINCODECHR_ERR(0xf0B)
#define WINCODEC_ERR_NOTINITIALIZED   MAKE_WINCODECHR_ERR(0xf0C)
#define WINCODEC_ERR_ALREADYLOCKED    MAKE_WINCODECHR_ERR(0xf0D)
#define WINCODEC_ERR_PROPERTYNOTFOUND  MAKE_WINCODECHR_ERR(0xf40)
#define WINCODEC_ERR_PROPERTYNOTSUPPORTED  MAKE_WINCODECHR_ERR(0xf41)
#define WINCODEC_ERR_PROPERTYSIZE  MAKE_WINCODECHR_ERR(0xf42)
#define WINCODEC_ERR_CODECPRESENT  MAKE_WINCODECHR_ERR(0xf43)
#define WINCODEC_ERR_CODECNOTHUMBNAIL  MAKE_WINCODECHR_ERR(0xf44)
#define WINCODEC_ERR_PALETTEUNAVAILABLE   MAKE_WINCODECHR_ERR(0xf45)
#define WINCODEC_ERR_CODECTOOMANYSCANLINES    MAKE_WINCODECHR_ERR(0xf46)
#define WINCODEC_ERR_INTERNALERROR    MAKE_WINCODECHR_ERR(0xf48)
#define WINCODEC_ERR_SOURCERECTDOESNOTMATCHDIMENSIONS MAKE_WINCODECHR_ERR(0xf49)
#define WINCODEC_ERR_COMPONENTNOTFOUND    MAKE_WINCODECHR_ERR(0xf50)
#define WINCODEC_ERR_IMAGESIZEOUTOFRANGE  MAKE_WINCODECHR_ERR(0xf51)
#define WINCODEC_ERR_TOOMUCHMETADATA  MAKE_WINCODECHR_ERR(0xf52)
#define WINCODEC_ERR_BADIMAGE  MAKE_WINCODECHR_ERR(0xf60)
#define WINCODEC_ERR_BADHEADER    MAKE_WINCODECHR_ERR(0xf61)
#define WINCODEC_ERR_FRAMEMISSING  MAKE_WINCODECHR_ERR(0xf62)
#define WINCODEC_ERR_BADMETADATAHEADER    MAKE_WINCODECHR_ERR(0xf63)
#define WINCODEC_ERR_BADSTREAMDATA    MAKE_WINCODECHR_ERR(0xf70)
#define WINCODEC_ERR_STREAMWRITE  MAKE_WINCODECHR_ERR(0xf71)
#define WINCODEC_ERR_STREAMREAD   MAKE_WINCODECHR_ERR(0xf72)
#define WINCODEC_ERR_STREAMNOTAVAILABLE   MAKE_WINCODECHR_ERR(0xf73)
#define WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT   MAKE_WINCODECHR_ERR(0xf80)
#define WINCODEC_ERR_UNSUPPORTEDOPERATION  MAKE_WINCODECHR_ERR(0xf81)
#define WINCODEC_ERR_INVALIDREGISTRATION  MAKE_WINCODECHR_ERR(0xf8A)
#define WINCODEC_ERR_COMPONENTINITIALIZEFAILURE   MAKE_WINCODECHR_ERR(0xf8B)
#define WINCODEC_ERR_INSUFFICIENTBUFFER   MAKE_WINCODECHR_ERR(0xf8C)
#define WINCODEC_ERR_DUPLICATEMETADATAPRESENT  MAKE_WINCODECHR_ERR(0xf8D)
#define WINCODEC_ERR_PROPERTYUNEXPECTEDTYPE   MAKE_WINCODECHR_ERR(0xf8E)
#define WINCODEC_ERR_UNEXPECTEDSIZE   MAKE_WINCODECHR_ERR(0xf8F)
#define WINCODEC_ERR_INVALIDQUERYREQUEST  MAKE_WINCODECHR_ERR(0xf90)
#define WINCODEC_ERR_UNEXPECTEDMETADATATYPE   MAKE_WINCODECHR_ERR(0xf91)
#define WINCODEC_ERR_REQUESTONLYVALIDATMETADATAROOT   MAKE_WINCODECHR_ERR(0xf92)
#define WINCODEC_ERR_INVALIDQUERYCHARACTER    MAKE_WINCODECHR_ERR(0xf93)
*/

typedef enum WICTiffCompressionOption {
    WICTiffCompressionDontCare = 0,
    WICTiffCompressionNone = 0x1,
    WICTiffCompressionCCITT3 = 0x2,
    WICTiffCompressionCCITT4 = 0x3,
    WICTiffCompressionLZW = 0x4,
    WICTiffCompressionRLE = 0x5,
    WICTiffCompressionZIP = 0x6,
    WICTIFFCOMPRESSIONOPTION_FORCE_DWORD = 0x7fffffff
} WICTiffCompressionOption;

typedef enum WICJpegYCrCbSubsamplingOption {
    WICJpegYCrCbSubsamplingDefault = 0,
    WICJpegYCrCbSubsampling420 = 0x1,
    WICJpegYCrCbSubsampling422 = 0x2,
    WICJpegYCrCbSubsampling444 = 0x3,
    WICJpegYCrCbSubsampling440 = 0x4,
    WICJPEGYCRCBSUBSAMPLING_FORCE_DWORD = 0x7fffffff
} WICJpegYCrCbSubsamplingOption;

typedef enum WICPngFilterOption {
    WICPngFilterUnspecified = 0,
    WICPngFilterNone = 0x1,
    WICPngFilterSub = 0x2,
    WICPngFilterUp = 0x3,
    WICPngFilterAverage = 0x4,
    WICPngFilterPaeth = 0x5,
    WICPngFilterAdaptive = 0x6,
    WICPNGFILTEROPTION_FORCE_DWORD = 0x7fffffff
} WICPngFilterOption;

typedef enum WICNamedWhitePoint {
    WICWhitePointDefault = 0x1,
    WICWhitePointDaylight = 0x2,
    WICWhitePointCloudy = 0x4,
    WICWhitePointShade = 0x8,
    WICWhitePointTungsten = 0x10,
    WICWhitePointFluorescent = 0x20,
    WICWhitePointFlash = 0x40,
    WICWhitePointUnderwater = 0x80,
    WICWhitePointCustom = 0x100,
    WICWhitePointAutoWhiteBalance = 0x200,
    WICWhitePointAsShot = WICWhitePointDefault,
    WICNAMEDWHITEPOINT_FORCE_DWORD = 0x7fffffff
} WICNamedWhitePoint;

typedef enum WICRawCapabilities {
    WICRawCapabilityNotSupported = 0,
    WICRawCapabilityGetSupported = 0x1,
    WICRawCapabilityFullySupported = 0x2,
    WICRAWCAPABILITIES_FORCE_DWORD = 0x7fffffff
} WICRawCapabilities;

typedef enum WICRawRotationCapabilities {
    WICRawRotationCapabilityNotSupported = 0,
    WICRawRotationCapabilityGetSupported = 0x1,
    WICRawRotationCapabilityNinetyDegreesSupported = 0x2,
    WICRawRotationCapabilityFullySupported = 0x3,
    WICRAWROTATIONCAPABILITIES_FORCE_DWORD = 0x7fffffff
} WICRawRotationCapabilities;

typedef struct WICRawCapabilitiesInfo {
    UINT cbSize;
    UINT CodecMajorVersion;
    UINT CodecMinorVersion;
    WICRawCapabilities ExposureCompensationSupport;
    WICRawCapabilities ContrastSupport;
    WICRawCapabilities RGBWhitePointSupport;
    WICRawCapabilities NamedWhitePointSupport;
    UINT NamedWhitePointSupportMask;
    WICRawCapabilities KelvinWhitePointSupport;
    WICRawCapabilities GammaSupport;
    WICRawCapabilities TintSupport;
    WICRawCapabilities SaturationSupport;
    WICRawCapabilities SharpnessSupport;
    WICRawCapabilities NoiseReductionSupport;
    WICRawCapabilities DestinationColorProfileSupport;
    WICRawCapabilities ToneCurveSupport;
    WICRawRotationCapabilities RotationSupport;
    WICRawCapabilities RenderModeSupport;
} WICRawCapabilitiesInfo;

typedef enum WICRawParameterSet {
    WICAsShotParameterSet = 0x1,
    WICUserAdjustedParameterSet = 0x2,
    WICAutoAdjustedParameterSet = 0x3,
    WICRAWPARAMETERSET_FORCE_DWORD = 0x7fffffff
} WICRawParameterSet;

typedef enum WICRawRenderMode {
    WICRawRenderModeDraft = 0x1,
    WICRawRenderModeNormal = 0x2,
    WICRawRenderModeBestQuality = 0x3,
    WICRAWRENDERMODE_FORCE_DWORD = 0x7fffffff
} WICRawRenderMode;

typedef struct WICRawToneCurvePoint {
    double Input;
    double Output;
} WICRawToneCurvePoint;

typedef struct WICRawToneCurve {
    UINT cPoints;
    WICRawToneCurvePoint aPoints[1];
} WICRawToneCurve;

#define WICRawChangeNotification_ExposureCompensation   0x00000001
#define WICRawChangeNotification_NamedWhitePoint    0x00000002
#define WICRawChangeNotification_KelvinWhitePoint   0x00000004
#define WICRawChangeNotification_RGBWhitePoint  0x00000008
#define WICRawChangeNotification_Contrast   0x00000010
#define WICRawChangeNotification_Gamma  0x00000020
#define WICRawChangeNotification_Sharpness  0x00000040
#define WICRawChangeNotification_Saturation  0x00000080
#define WICRawChangeNotification_Tint   0x00000100
#define WICRawChangeNotification_NoiseReduction  0x00000200
#define WICRawChangeNotification_DestinationColorContext  0x00000400
#define WICRawChangeNotification_ToneCurve  0x00000800
#define WICRawChangeNotification_Rotation   0x00001000
#define WICRawChangeNotification_RenderMode  0x00002000

extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0028_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0028_v0_0_s_ifspec;

#ifndef __IWICDevelopRawNotificationCallback_INTERFACE_DEFINED__
#define __IWICDevelopRawNotificationCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICDevelopRawNotificationCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("95c75a6e-3e8c-4ec2-85a8-aebcc551e59b")IWICDevelopRawNotificationCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Notify(UINT NotificationMask) = 0;
};
#else /* C style interface */
typedef struct IWICDevelopRawNotificationCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICDevelopRawNotificationCallback * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICDevelopRawNotificationCallback * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICDevelopRawNotificationCallback * This);
    HRESULT(STDMETHODCALLTYPE *Notify)(IWICDevelopRawNotificationCallback * This, UINT NotificationMask);
    END_INTERFACE
} IWICDevelopRawNotificationCallbackVtbl;

interface IWICDevelopRawNotificationCallback {
    CONST_VTBL struct IWICDevelopRawNotificationCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICDevelopRawNotificationCallback_QueryInterface(This,riid,ppvObject)    ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICDevelopRawNotificationCallback_AddRef(This)   ((This)->lpVtbl->AddRef(This))
#define IWICDevelopRawNotificationCallback_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICDevelopRawNotificationCallback_Notify(This,NotificationMask)  ((This)->lpVtbl->Notify(This,NotificationMask))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICDevelopRawNotificationCallback_INTERFACE_DEFINED__ */

#ifndef __IWICDevelopRaw_INTERFACE_DEFINED__
#define __IWICDevelopRaw_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICDevelopRaw;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fbec5e44-f7be-4b65-b7f8-c0c81fef026d")IWICDevelopRaw:public IWICBitmapFrameDecode
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryRawCapabilitiesInfo(WICRawCapabilitiesInfo * pInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE LoadParameterSet(WICRawParameterSet ParameterSet) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentParameterSet(IPropertyBag2 ** ppCurrentParameterSet) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetExposureCompensation(double ev) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExposureCompensation(double *pEV) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetWhitePointRGB(UINT Red, UINT Green, UINT Blue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWhitePointRGB(UINT * pRed, UINT * pGreen, UINT * pBlue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNamedWhitePoint(WICNamedWhitePoint WhitePoint) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedWhitePoint(WICNamedWhitePoint * pWhitePoint) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetWhitePointKelvin(UINT WhitePointKelvin) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWhitePointKelvin(UINT * pWhitePointKelvin) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetKelvinRangeInfo(UINT * pMinKelvinTemp, UINT * pMaxKelvinTemp, UINT * pKelvinTempStepValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetContrast(double Contrast) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContrast(double *pContrast) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGamma(double Gamma) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGamma(double *pGamma) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSharpness(double Sharpness) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSharpness(double *pSharpness) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSaturation(double Saturation) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSaturation(double *pSaturation) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTint(double Tint) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTint(double *pTint) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNoiseReduction(double NoiseReduction) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNoiseReduction(double *pNoiseReduction) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDestinationColorContext(const IWICColorContext * pColorContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetToneCurve(UINT cbToneCurveSize, const WICRawToneCurve * pToneCurve) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetToneCurve(UINT cbToneCurveBufferSize, WICRawToneCurve * pToneCurve, UINT * pcbActualToneCurveBufferSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRotation(double Rotation) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRotation(double *pRotation) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRenderMode(WICRawRenderMode RenderMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRenderMode(WICRawRenderMode * pRenderMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNotificationCallback(IWICDevelopRawNotificationCallback * pCallback) = 0;
};
#else /* C style interface */
typedef struct IWICDevelopRawVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IWICDevelopRaw * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWICDevelopRaw * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWICDevelopRaw * This);
    HRESULT(STDMETHODCALLTYPE *GetSize)(IWICDevelopRaw * This, UINT * puiWidth, UINT * puiHeight);
    HRESULT(STDMETHODCALLTYPE *GetPixelFormat)(IWICDevelopRaw * This, WICPixelFormatGUID * pPixelFormat);
    HRESULT(STDMETHODCALLTYPE *GetResolution)(IWICDevelopRaw * This, double *pDpiX, double *pDpiY);
    HRESULT(STDMETHODCALLTYPE *CopyPalette)(IWICDevelopRaw * This, IWICPalette * pIPalette);
    HRESULT(STDMETHODCALLTYPE *CopyPixels)(IWICDevelopRaw * This, const WICRect * prc, UINT cbStride, UINT cbBufferSize, BYTE * pbBuffer);
    HRESULT(STDMETHODCALLTYPE *GetMetadataQueryReader)(IWICDevelopRaw * This, IWICMetadataQueryReader ** ppIMetadataQueryReader);
    HRESULT(STDMETHODCALLTYPE *GetColorContexts)(IWICDevelopRaw * This, UINT cCount, IWICColorContext ** ppIColorContexts, UINT * pcActualCount);
    HRESULT(STDMETHODCALLTYPE *GetThumbnail)(IWICDevelopRaw * This, IWICBitmapSource ** ppIThumbnail);
    HRESULT(STDMETHODCALLTYPE *QueryRawCapabilitiesInfo)(IWICDevelopRaw * This, WICRawCapabilitiesInfo * pInfo);
    HRESULT(STDMETHODCALLTYPE *LoadParameterSet)(IWICDevelopRaw * This, WICRawParameterSet ParameterSet);
    HRESULT(STDMETHODCALLTYPE *GetCurrentParameterSet)(IWICDevelopRaw * This, IPropertyBag2 ** ppCurrentParameterSet);
    HRESULT(STDMETHODCALLTYPE *SetExposureCompensation)(IWICDevelopRaw * This, double ev);
    HRESULT(STDMETHODCALLTYPE *GetExposureCompensation)(IWICDevelopRaw * This, double *pEV);
    HRESULT(STDMETHODCALLTYPE *SetWhitePointRGB)(IWICDevelopRaw * This, UINT Red, UINT Green, UINT Blue);
    HRESULT(STDMETHODCALLTYPE *GetWhitePointRGB)(IWICDevelopRaw * This, UINT * pRed, UINT * pGreen, UINT * pBlue);
    HRESULT(STDMETHODCALLTYPE *SetNamedWhitePoint)(IWICDevelopRaw * This, WICNamedWhitePoint WhitePoint);
    HRESULT(STDMETHODCALLTYPE *GetNamedWhitePoint)(IWICDevelopRaw * This, WICNamedWhitePoint * pWhitePoint);
    HRESULT(STDMETHODCALLTYPE *SetWhitePointKelvin)(IWICDevelopRaw * This, UINT WhitePointKelvin);
    HRESULT(STDMETHODCALLTYPE *GetWhitePointKelvin)(IWICDevelopRaw * This, UINT * pWhitePointKelvin);
    HRESULT(STDMETHODCALLTYPE *GetKelvinRangeInfo)(IWICDevelopRaw * This, UINT * pMinKelvinTemp, UINT * pMaxKelvinTemp, UINT * pKelvinTempStepValue);
    HRESULT(STDMETHODCALLTYPE *SetContrast)(IWICDevelopRaw * This, double Contrast);
    HRESULT(STDMETHODCALLTYPE *GetContrast)(IWICDevelopRaw * This, double *pContrast);
    HRESULT(STDMETHODCALLTYPE *SetGamma)(IWICDevelopRaw * This, double Gamma);
    HRESULT(STDMETHODCALLTYPE *GetGamma)(IWICDevelopRaw * This, double *pGamma);
    HRESULT(STDMETHODCALLTYPE *SetSharpness)(IWICDevelopRaw * This, double Sharpness);
    HRESULT(STDMETHODCALLTYPE *GetSharpness)(IWICDevelopRaw * This, double *pSharpness);
    HRESULT(STDMETHODCALLTYPE *SetSaturation)(IWICDevelopRaw * This, double Saturation);
    HRESULT(STDMETHODCALLTYPE *GetSaturation)(IWICDevelopRaw * This, double *pSaturation);
    HRESULT(STDMETHODCALLTYPE *SetTint)(IWICDevelopRaw * This, double Tint);
    HRESULT(STDMETHODCALLTYPE *GetTint)(IWICDevelopRaw * This, double *pTint);
    HRESULT(STDMETHODCALLTYPE *SetNoiseReduction)(IWICDevelopRaw * This, double NoiseReduction);
    HRESULT(STDMETHODCALLTYPE *GetNoiseReduction)(IWICDevelopRaw * This, double *pNoiseReduction);
    HRESULT(STDMETHODCALLTYPE *SetDestinationColorContext)(IWICDevelopRaw * This, const IWICColorContext * pColorContext);
    HRESULT(STDMETHODCALLTYPE *SetToneCurve)(IWICDevelopRaw * This, UINT cbToneCurveSize, const WICRawToneCurve * pToneCurve);
    HRESULT(STDMETHODCALLTYPE *GetToneCurve)(IWICDevelopRaw * This, UINT cbToneCurveBufferSize, WICRawToneCurve * pToneCurve, UINT * pcbActualToneCurveBufferSize);
    HRESULT(STDMETHODCALLTYPE *SetRotation)(IWICDevelopRaw * This, double Rotation);
    HRESULT(STDMETHODCALLTYPE *GetRotation)(IWICDevelopRaw * This, double *pRotation);
    HRESULT(STDMETHODCALLTYPE *SetRenderMode)(IWICDevelopRaw * This, WICRawRenderMode RenderMode);
    HRESULT(STDMETHODCALLTYPE *GetRenderMode)(IWICDevelopRaw * This, WICRawRenderMode * pRenderMode);
    HRESULT(STDMETHODCALLTYPE *SetNotificationCallback)(IWICDevelopRaw * This, IWICDevelopRawNotificationCallback * pCallback);
    END_INTERFACE
} IWICDevelopRawVtbl;

interface IWICDevelopRaw {
    CONST_VTBL struct IWICDevelopRawVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICDevelopRaw_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICDevelopRaw_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICDevelopRaw_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICDevelopRaw_GetSize(This,puiWidth,puiHeight)  ((This)->lpVtbl->GetSize(This,puiWidth,puiHeight))
#define IWICDevelopRaw_GetPixelFormat(This,pPixelFormat)  ((This)->lpVtbl->GetPixelFormat(This,pPixelFormat))
#define IWICDevelopRaw_GetResolution(This,pDpiX,pDpiY)  ((This)->lpVtbl->GetResolution(This,pDpiX,pDpiY))
#define IWICDevelopRaw_CopyPalette(This,pIPalette)  ((This)->lpVtbl->CopyPalette(This,pIPalette))
#define IWICDevelopRaw_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer))
#define IWICDevelopRaw_GetMetadataQueryReader(This,ppIMetadataQueryReader)  ((This)->lpVtbl->GetMetadataQueryReader(This,ppIMetadataQueryReader))
#define IWICDevelopRaw_GetColorContexts(This,cCount,ppIColorContexts,pcActualCount)   ((This)->lpVtbl->GetColorContexts(This,cCount,ppIColorContexts,pcActualCount))
#define IWICDevelopRaw_GetThumbnail(This,ppIThumbnail)  ((This)->lpVtbl->GetThumbnail(This,ppIThumbnail))
#define IWICDevelopRaw_QueryRawCapabilitiesInfo(This,pInfo)  ((This)->lpVtbl->QueryRawCapabilitiesInfo(This,pInfo))
#define IWICDevelopRaw_LoadParameterSet(This,ParameterSet)  ((This)->lpVtbl->LoadParameterSet(This,ParameterSet))
#define IWICDevelopRaw_GetCurrentParameterSet(This,ppCurrentParameterSet)  ((This)->lpVtbl->GetCurrentParameterSet(This,ppCurrentParameterSet))
#define IWICDevelopRaw_SetExposureCompensation(This,ev)  ((This)->lpVtbl->SetExposureCompensation(This,ev))
#define IWICDevelopRaw_GetExposureCompensation(This,pEV)  ((This)->lpVtbl->GetExposureCompensation(This,pEV))
#define IWICDevelopRaw_SetWhitePointRGB(This,Red,Green,Blue)  ((This)->lpVtbl->SetWhitePointRGB(This,Red,Green,Blue))
#define IWICDevelopRaw_GetWhitePointRGB(This,pRed,pGreen,pBlue)   ((This)->lpVtbl->GetWhitePointRGB(This,pRed,pGreen,pBlue))
#define IWICDevelopRaw_SetNamedWhitePoint(This,WhitePoint)  ((This)->lpVtbl->SetNamedWhitePoint(This,WhitePoint))
#define IWICDevelopRaw_GetNamedWhitePoint(This,pWhitePoint)  ((This)->lpVtbl->GetNamedWhitePoint(This,pWhitePoint))
#define IWICDevelopRaw_SetWhitePointKelvin(This,WhitePointKelvin)  ((This)->lpVtbl->SetWhitePointKelvin(This,WhitePointKelvin))
#define IWICDevelopRaw_GetWhitePointKelvin(This,pWhitePointKelvin)  ((This)->lpVtbl->GetWhitePointKelvin(This,pWhitePointKelvin))
#define IWICDevelopRaw_GetKelvinRangeInfo(This,pMinKelvinTemp,pMaxKelvinTemp,pKelvinTempStepValue)    ((This)->lpVtbl->GetKelvinRangeInfo(This,pMinKelvinTemp,pMaxKelvinTemp,pKelvinTempStepValue))
#define IWICDevelopRaw_SetContrast(This,Contrast)  ((This)->lpVtbl->SetContrast(This,Contrast))
#define IWICDevelopRaw_GetContrast(This,pContrast)  ((This)->lpVtbl->GetContrast(This,pContrast))
#define IWICDevelopRaw_SetGamma(This,Gamma)  ((This)->lpVtbl->SetGamma(This,Gamma))
#define IWICDevelopRaw_GetGamma(This,pGamma)  ((This)->lpVtbl->GetGamma(This,pGamma))
#define IWICDevelopRaw_SetSharpness(This,Sharpness)  ((This)->lpVtbl->SetSharpness(This,Sharpness))
#define IWICDevelopRaw_GetSharpness(This,pSharpness)  ((This)->lpVtbl->GetSharpness(This,pSharpness))
#define IWICDevelopRaw_SetSaturation(This,Saturation)  ((This)->lpVtbl->SetSaturation(This,Saturation))
#define IWICDevelopRaw_GetSaturation(This,pSaturation)  ((This)->lpVtbl->GetSaturation(This,pSaturation))
#define IWICDevelopRaw_SetTint(This,Tint)  ((This)->lpVtbl->SetTint(This,Tint))
#define IWICDevelopRaw_GetTint(This,pTint)  ((This)->lpVtbl->GetTint(This,pTint))
#define IWICDevelopRaw_SetNoiseReduction(This,NoiseReduction)  ((This)->lpVtbl->SetNoiseReduction(This,NoiseReduction))
#define IWICDevelopRaw_GetNoiseReduction(This,pNoiseReduction)  ((This)->lpVtbl->GetNoiseReduction(This,pNoiseReduction))
#define IWICDevelopRaw_SetDestinationColorContext(This,pColorContext)  ((This)->lpVtbl->SetDestinationColorContext(This,pColorContext))
#define IWICDevelopRaw_SetToneCurve(This,cbToneCurveSize,pToneCurve)  ((This)->lpVtbl->SetToneCurve(This,cbToneCurveSize,pToneCurve))
#define IWICDevelopRaw_GetToneCurve(This,cbToneCurveBufferSize,pToneCurve,pcbActualToneCurveBufferSize)   ((This)->lpVtbl->GetToneCurve(This,cbToneCurveBufferSize,pToneCurve,pcbActualToneCurveBufferSize))
#define IWICDevelopRaw_SetRotation(This,Rotation)  ((This)->lpVtbl->SetRotation(This,Rotation))
#define IWICDevelopRaw_GetRotation(This,pRotation)  ((This)->lpVtbl->GetRotation(This,pRotation))
#define IWICDevelopRaw_SetRenderMode(This,RenderMode)  ((This)->lpVtbl->SetRenderMode(This,RenderMode))
#define IWICDevelopRaw_GetRenderMode(This,pRenderMode)  ((This)->lpVtbl->GetRenderMode(This,pRenderMode))
#define IWICDevelopRaw_SetNotificationCallback(This,pCallback)  ((This)->lpVtbl->SetNotificationCallback(This,pCallback))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICDevelopRaw_INTERFACE_DEFINED__ */

typedef enum WICDdsDimension {
    WICDdsTexture1D = 0,
    WICDdsTexture2D = 0x1,
    WICDdsTexture3D = 0x2,
    WICDdsTextureCube = 0x3,
    WICDDSTEXTURE_FORCE_DWORD = 0x7fffffff
} WICDdsDimension;

typedef enum WICDdsAlphaMode {
    WICDdsAlphaModeUnknown = 0,
    WICDdsAlphaModeStraight = 0x1,
    WICDdsAlphaModePremultiplied = 0x2,
    WICDdsAlphaModeOpaque = 0x3,
    WICDdsAlphaModeCustom = 0x4,
    WICDDSALPHAMODE_FORCE_DWORD = 0x7fffffff
} WICDdsAlphaMode;

typedef struct WICDdsParameters {
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    UINT ArraySize;
    DXGI_FORMAT DxgiFormat;
    WICDdsDimension Dimension;
    WICDdsAlphaMode AlphaMode;
}
WICDdsParameters;

extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0038_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0038_v0_0_s_ifspec;

#ifndef __IWICDdsDecoder_INTERFACE_DEFINED__
#define __IWICDdsDecoder_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICDdsDecoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("409cd537-8532-40cb-9774-e2feb2df4e9c")IWICDdsDecoder:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetParameters(WICDdsParameters *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFrame(UINT, UINT, UINT, IWICBitmapFrameDecode **) = 0;
};
#else /* C style interface */
typedef struct IWICDdsDecoderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICDdsDecoder *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICDdsDecoder *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICDdsDecoder *);
    HRESULT (STDMETHODCALLTYPE *GetParameters)(IWICDdsDecoder *, WICDdsParameters *);
    HRESULT (STDMETHODCALLTYPE *GetFrame)(IWICDdsDecoder *, UINT, UINT, UINT, IWICBitmapFrameDecode **);
    END_INTERFACE
} IWICDdsDecoderVtbl;

interface IWICDdsDecoder {
    CONST_VTBL struct IWICDdsDecoderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICDdsDecoder_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICDdsDecoder_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICDdsDecoder_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICDdsDecoder_GetParameters(This,pParameters)  ((This)->lpVtbl->GetParameters(This,pParameters))
#define IWICDdsDecoder_GetFrame(This,arrayIndex,mipLevel,sliceIndex,ppIBitmapFrame)  ((This)->lpVtbl->GetFrame(This,arrayIndex,mipLevel,sliceIndex,ppIBitmapFrame))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICDdsDecoder_INTERFACE_DEFINED__ */

#ifndef __IWICDdsEncoder_INTERFACE_DEFINED__
#define __IWICDdsEncoder_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICDdsEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5cacdb4c-407e-41b3-b936-d0f010cd6732")IWICDdsEncoder:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetParameters(WICDdsParameters *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParameters(WICDdsParameters *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateNewFrame(IWICBitmapFrameEncode **, UINT *, UINT *, UINT *) = 0;
};
#else /* C style interface */
typedef struct IWICDdsEncoderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICDdsEncoder *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICDdsEncoder *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICDdsEncoder *);
    HRESULT (STDMETHODCALLTYPE *SetParameters)(IWICDdsEncoder *, WICDdsParameters *);
    HRESULT (STDMETHODCALLTYPE *GetParameters)(IWICDdsEncoder *, WICDdsParameters *);
    HRESULT (STDMETHODCALLTYPE *CreateNewFrame)(IWICDdsEncoder *, IWICBitmapFrameEncode **, UINT *, UINT *, UINT *);
    END_INTERFACE
} IWICDdsEncoderVtbl;

interface IWICDdsEncoder {
    CONST_VTBL struct IWICDdsEncoderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICDdsEncoder_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICDdsEncoder_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICDdsEncoder_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICDdsEncoder_SetParameters(This,pParameters)  ((This)->lpVtbl->SetParameters(This,pParameters))
#define IWICDdsEncoder_GetParameters(This,pParameters)  ((This)->lpVtbl->GetParameters(This,pParameters))
#define IWICDdsEncoder_CreateNewFrame(This,ppIFrameEncode,pArrayIndex,pMipLevel,pSliceIndex)  ((This)->lpVtbl->CreateNewFrame(This,ppIFrameEncode,pArrayIndex,pMipLevel,pSliceIndex))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICDdsEncoder_INTERFACE_DEFINED__ */

typedef struct WICDdsFormatInfo {
    DXGI_FORMAT DxgiFormat;
    UINT BytesPerBlock;
    UINT BlockWidth;
    UINT BlockHeight;
} WICDdsFormatInfo;

extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0040_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0040_v0_0_s_ifspec;

#ifndef __IWICDdsFrameDecode_INTERFACE_DEFINED__
#define __IWICDdsFrameDecode_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWICDdsFrameDecode;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3d4c0c61-18a4-41e4-bd80-481a4fc9f464")IWICDdsFrameDecode:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSizeInBlocks(UINT *, UINT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFormatInfo(WICDdsFormatInfo *) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyBlocks(const WICRect *, UINT, UINT, BYTE *) = 0;
};
#else /* C style interface */
typedef struct IWICDdsFrameDecodeVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWICDdsFrameDecode *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWICDdsFrameDecode *);
    ULONG (STDMETHODCALLTYPE *Release)(IWICDdsFrameDecode *);
    HRESULT (STDMETHODCALLTYPE *GetSizeInBlocks)(IWICDdsFrameDecode *, UINT *, UINT *);
    HRESULT (STDMETHODCALLTYPE *GetFormatInfo)(IWICDdsFrameDecode *, WICDdsFormatInfo *);
    HRESULT (STDMETHODCALLTYPE *CopyBlocks)(IWICDdsFrameDecode *, const WICRect *, UINT, UINT, BYTE *);
    END_INTERFACE
} IWICDdsFrameDecodeVtbl;

interface IWICDdsFrameDecode {
    CONST_VTBL struct IWICDdsFrameDecodeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWICDdsFrameDecode_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWICDdsFrameDecode_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWICDdsFrameDecode_Release(This)  ((This)->lpVtbl->Release(This))
#define IWICDdsFrameDecode_GetSizeInBlocks(This,pWidthInBlocks,pHeightInBlocks)  ((This)->lpVtbl->GetSizeInBlocks(This,pWidthInBlocks,pHeightInBlocks))
#define IWICDdsFrameDecode_GetFormatInfo(This,pFormatInfo)  ((This)->lpVtbl->GetFormatInfo(This,pFormatInfo))
#define IWICDdsFrameDecode_CopyBlocks(This,prcBoundsInBlocks,cbStride,cbBufferSize,pbBuffer)  ((This)->lpVtbl->CopyBlocks(This,prcBoundsInBlocks,cbStride,cbBufferSize,pbBuffer))

#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWICDdsFrameDecode_INTERFACE_DEFINED__ */

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER HBITMAP_UserSize(unsigned long *, unsigned long, HBITMAP *);
unsigned char *__RPC_USER HBITMAP_UserMarshal(unsigned long *, unsigned char *, HBITMAP *);
unsigned char *__RPC_USER HBITMAP_UserUnmarshal(unsigned long *, unsigned char *, HBITMAP *);
void __RPC_USER HBITMAP_UserFree(unsigned long *, HBITMAP *);

unsigned long __RPC_USER HICON_UserSize(unsigned long *, unsigned long, HICON *);
unsigned char *__RPC_USER HICON_UserMarshal(unsigned long *, unsigned char *, HICON *);
unsigned char *__RPC_USER HICON_UserUnmarshal(unsigned long *, unsigned char *, HICON *);
void __RPC_USER HICON_UserFree(unsigned long *, HICON *);

unsigned long __RPC_USER HPALETTE_UserSize(unsigned long *, unsigned long, HPALETTE *);
unsigned char *__RPC_USER HPALETTE_UserMarshal(unsigned long *, unsigned char *, HPALETTE *);
unsigned char *__RPC_USER HPALETTE_UserUnmarshal(unsigned long *, unsigned char *, HPALETTE *);
void __RPC_USER HPALETTE_UserFree(unsigned long *, HPALETTE *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree(unsigned long *, LPSAFEARRAY *);

HRESULT STDMETHODCALLTYPE IWICBitmapLock_GetDataPointer_Proxy(IWICBitmapLock * This, UINT * pcbBufferSize, BYTE ** ppbData);
HRESULT STDMETHODCALLTYPE IWICBitmapLock_GetDataPointer_Stub(IWICBitmapLock * This, UINT * pcbBufferSize, BYTE ** ppbData);

HRESULT STDMETHODCALLTYPE IWICBitmapCodecProgressNotification_RegisterProgressNotification_Proxy(IWICBitmapCodecProgressNotification *, PFNProgressNotification, LPVOID, DWORD);
HRESULT STDMETHODCALLTYPE IWICBitmapCodecProgressNotification_RegisterProgressNotification_Stub(IWICBitmapCodecProgressNotification *, IWICProgressCallback *, DWORD);
HRESULT STDMETHODCALLTYPE IWICBitmapDecoderInfo_GetPatterns_Proxy(IWICBitmapDecoderInfo *, UINT, WICBitmapPattern *, UINT *, UINT *);
HRESULT STDMETHODCALLTYPE IWICBitmapDecoderInfo_GetPatterns_Stub(IWICBitmapDecoderInfo *, WICBitmapPattern **, UINT *);
HRESULT STDMETHODCALLTYPE IWICDevelopRaw_QueryRawCapabilitiesInfo_Proxy(IWICDevelopRaw *, WICRawCapabilitiesInfo *);
HRESULT STDMETHODCALLTYPE IWICDevelopRaw_QueryRawCapabilitiesInfo_Stub(IWICDevelopRaw *, WICRawCapabilitiesInfo *);
HRESULT STDMETHODCALLTYPE IWICDevelopRaw_SetToneCurve_Proxy(IWICDevelopRaw *, UINT, const WICRawToneCurve *);
HRESULT STDMETHODCALLTYPE IWICDevelopRaw_SetToneCurve_Stub(IWICDevelopRaw *, UINT, const WICRawToneCurvePoint *);
HRESULT STDMETHODCALLTYPE IWICDevelopRaw_GetToneCurve_Proxy(IWICDevelopRaw *, UINT, WICRawToneCurve *, UINT *);
HRESULT STDMETHODCALLTYPE IWICDevelopRaw_GetToneCurve_Stub(IWICDevelopRaw *, UINT *, WICRawToneCurvePoint **);


#ifdef __cplusplus
}
#endif

#endif
