/* 
   winiostr.h

   Base definitions

   Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   If you are interested in a warranty or support for this source code,
   contact Scott Christley <scottc@net-community.com> for more information.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 

#ifndef _NTDDSTOR_H_
#define _NTDDSTOR_H_

#pragma pack(8)

typedef struct _STORAGE_DEVICE_NUMBER {
    DEVICE_TYPE DeviceType;
    DWORD       DeviceNumber;
    DWORD       PartitionNumber;
} STORAGE_DEVICE_NUMBER, *PSTORAGE_DEVICE_NUMBER;

typedef struct _STORAGE_BUS_RESET_REQUEST {
    BYTE  PathId;
} STORAGE_BUS_RESET_REQUEST, *PSTORAGE_BUS_RESET_REQUEST;

typedef struct _PREVENT_MEDIA_REMOVAL {
    BOOLEAN PreventMediaRemoval;
} PREVENT_MEDIA_REMOVAL, *PPREVENT_MEDIA_REMOVAL;

typedef struct _TAPE_STATISTICS {
    DWORD Version;
    DWORD Flags;
    LARGE_INTEGER RecoveredWrites;
    LARGE_INTEGER UnrecoveredWrites;
    LARGE_INTEGER RecoveredReads;
    LARGE_INTEGER UnrecoveredReads;
    BYTE          CompressionRatioReads;
    BYTE          CompressionRatioWrites;
} TAPE_STATISTICS, *PTAPE_STATISTICS;

typedef struct _TAPE_GET_STATISTICS {
    DWORD Operation;
} TAPE_GET_STATISTICS, *PTAPE_GET_STATISTICS;

typedef enum _STORAGE_MEDIA_TYPE {
    DDS_4mm = 0x20,            
    MiniQic,                   
    Travan,                    
    QIC,                       
    MP_8mm,                    
    AME_8mm,                   
    AIT1_8mm,                  
    DLT,                       
    NCTP,                      
    IBM_3480,                  
    IBM_3490E,                 
    IBM_Magstar_3590,          
    IBM_Magstar_MP,            
    STK_DATA_D3,               
    SONY_DTF,                  
    DV_6mm,                    
    DMI,                       
    SONY_D2,                   
    CLEANER_CARTRIDGE,         
    CD_ROM,                    
    CD_R,                      
    CD_RW,                     
    DVD_ROM,                   
    DVD_R,                     
    DVD_RW,                    
    MO_3_RW,                   
    MO_5_WO,                   
    MO_5_RW,                   
    MO_5_LIMDOW,               
    PC_5_WO,                   
    PC_5_RW,                   
    PD_5_RW,                   
    ABL_5_WO,                  
    PINNACLE_APEX_5_RW,        
    SONY_12_WO,                
    PHILIPS_12_WO,             
    HITACHI_12_WO,             
    CYGNET_12_WO,              
    KODAK_14_WO,               
    MO_NFR_525,                
    NIKON_12_RW,               
    IOMEGA_ZIP,                
    IOMEGA_JAZ,                
    SYQUEST_EZ135,             
    SYQUEST_EZFLYER,           
    SYQUEST_SYJET,             
    AVATAR_F2,                 
    MP2_8mm,                   
    DST_S,                     
    DST_M,                     
    DST_L,                     
    VXATape_1,                 
    VXATape_2,                 
    STK_EAGLE,                 
    LTO_Ultrium,               
    LTO_Accelis                
} STORAGE_MEDIA_TYPE, *PSTORAGE_MEDIA_TYPE;

typedef enum _STORAGE_BUS_TYPE {
    BusTypeUnknown = 0x00,
    BusTypeScsi,
    BusTypeAtapi,
    BusTypeAta,
    BusType1394,
    BusTypeSsa,
    BusTypeFibre,
    BusTypeUsb,
    BusTypeRAID,
    BusTypeMaxReserved = 0x7F
} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;

typedef struct _DEVICE_MEDIA_INFO {
    union {
        struct {
            LARGE_INTEGER Cylinders;
            STORAGE_MEDIA_TYPE MediaType;
            DWORD TracksPerCylinder;
            DWORD SectorsPerTrack;
            DWORD BytesPerSector;
            DWORD NumberMediaSides;
            DWORD MediaCharacteristics; 
        } DiskInfo;

        struct {
            LARGE_INTEGER Cylinders;
            STORAGE_MEDIA_TYPE MediaType;
            DWORD TracksPerCylinder;
            DWORD SectorsPerTrack;
            DWORD BytesPerSector;
            DWORD NumberMediaSides;
            DWORD MediaCharacteristics; 
        } RemovableDiskInfo;

        struct {
            STORAGE_MEDIA_TYPE MediaType;
            DWORD   MediaCharacteristics; 
            DWORD   CurrentBlockSize;
            STORAGE_BUS_TYPE BusType;
            union {
                struct {
                    BYTE  MediumType;
                    BYTE  DensityCode;
                } ScsiInformation;
            } BusSpecificData;

        } TapeInfo;
    } DeviceSpecific;
} DEVICE_MEDIA_INFO, *PDEVICE_MEDIA_INFO;

typedef struct _GET_MEDIA_TYPES {
    DWORD DeviceType;              
    DWORD MediaInfoCount;
    DEVICE_MEDIA_INFO MediaInfo[1];
} GET_MEDIA_TYPES, *PGET_MEDIA_TYPES;

typedef struct _STORAGE_PREDICT_FAILURE
{
    DWORD PredictFailure;
    BYTE  VendorSpecific[512];
} STORAGE_PREDICT_FAILURE, *PSTORAGE_PREDICT_FAILURE;

#endif 

typedef enum _MEDIA_TYPE {
    Unknown,                
    F5_1Pt2_512,            
    F3_1Pt44_512,           
    F3_2Pt88_512,           
    F3_20Pt8_512,           
    F3_720_512,             
    F5_360_512,             
    F5_320_512,             
    F5_320_1024,            
    F5_180_512,             
    F5_160_512,             
    RemovableMedia,         
    FixedMedia,             
    F3_120M_512,            
    F3_640_512,             
    F5_640_512,             
    F5_720_512,             
    F3_1Pt2_512,            
    F3_1Pt23_1024,          
    F5_1Pt23_1024,          
    F3_128Mb_512,           
    F3_230Mb_512,           
    F8_256_128              
} MEDIA_TYPE, *PMEDIA_TYPE;

typedef struct _FORMAT_PARAMETERS {
   MEDIA_TYPE MediaType;
   DWORD StartCylinderNumber;
   DWORD EndCylinderNumber;
   DWORD StartHeadNumber;
   DWORD EndHeadNumber;
} FORMAT_PARAMETERS, *PFORMAT_PARAMETERS;

typedef WORD   BAD_TRACK_NUMBER;
typedef WORD   *PBAD_TRACK_NUMBER;

typedef struct _FORMAT_EX_PARAMETERS {
   MEDIA_TYPE MediaType;
   DWORD StartCylinderNumber;
   DWORD EndCylinderNumber;
   DWORD StartHeadNumber;
   DWORD EndHeadNumber;
   WORD   FormatGapLength;
   WORD   SectorsPerTrack;
   WORD   SectorNumber[1];
} FORMAT_EX_PARAMETERS, *PFORMAT_EX_PARAMETERS;

typedef struct _DISK_GEOMETRY {
    LARGE_INTEGER Cylinders;
    MEDIA_TYPE MediaType;
    DWORD TracksPerCylinder;
    DWORD SectorsPerTrack;
    DWORD BytesPerSector;
} DISK_GEOMETRY, *PDISK_GEOMETRY;

typedef struct _PARTITION_INFORMATION {
    LARGE_INTEGER StartingOffset;
    LARGE_INTEGER PartitionLength;
    DWORD HiddenSectors;
    DWORD PartitionNumber;
    BYTE  PartitionType;
    BOOLEAN BootIndicator;
    BOOLEAN RecognizedPartition;
    BOOLEAN RewritePartition;
} PARTITION_INFORMATION, *PPARTITION_INFORMATION;

typedef struct _SET_PARTITION_INFORMATION {
    BYTE  PartitionType;
} SET_PARTITION_INFORMATION, *PSET_PARTITION_INFORMATION;

typedef struct _DRIVE_LAYOUT_INFORMATION {
    DWORD PartitionCount;
    DWORD Signature;
    PARTITION_INFORMATION PartitionEntry[1];
} DRIVE_LAYOUT_INFORMATION, *PDRIVE_LAYOUT_INFORMATION;

typedef struct _VERIFY_INFORMATION {
    LARGE_INTEGER StartingOffset;
    DWORD Length;
} VERIFY_INFORMATION, *PVERIFY_INFORMATION;

typedef struct _REASSIGN_BLOCKS {
    WORD   Reserved;
    WORD   Count;
    DWORD BlockNumber[1];
} REASSIGN_BLOCKS, *PREASSIGN_BLOCKS;

typedef struct _DISK_CONTROLLER_NUMBER {
    DWORD ControllerNumber;
    DWORD DiskNumber;
} DISK_CONTROLLER_NUMBER, *PDISK_CONTROLLER_NUMBER;

typedef enum {
    EqualPriority,
    KeepPrefetchedData,
    KeepReadData
} DISK_CACHE_RETENTION_PRIORITY;

typedef struct _DISK_CACHE_INFORMATION {
    BOOLEAN ParametersSavable;
    BOOLEAN ReadCacheEnabled;
    BOOLEAN WriteCacheEnabled;
    DISK_CACHE_RETENTION_PRIORITY ReadRetentionPriority;
    DISK_CACHE_RETENTION_PRIORITY WriteRetentionPriority;
    WORD   DisablePrefetchTransferLength;
    BOOLEAN PrefetchScalar;
    union {
        struct {
            WORD   Minimum;
            WORD   Maximum;
            WORD   MaximumBlocks;
        } ScalarPrefetch;

        struct {
            WORD   Minimum;
            WORD   Maximum;
        } BlockPrefetch;
    };

} DISK_CACHE_INFORMATION, *PDISK_CACHE_INFORMATION;

typedef struct _DISK_GROW_PARTITION {
    DWORD PartitionNumber;
    LARGE_INTEGER BytesToGrow;
} DISK_GROW_PARTITION, *PDISK_GROW_PARTITION;

typedef struct _HISTOGRAM_BUCKET {
    DWORD       Reads;
    DWORD       Writes;
} HISTOGRAM_BUCKET, *PHISTOGRAM_BUCKET;

typedef struct _DISK_HISTOGRAM {
    LARGE_INTEGER   DiskSize;
    LARGE_INTEGER   Start;
    LARGE_INTEGER   End;
    LARGE_INTEGER   Average;
    LARGE_INTEGER   AverageRead;
    LARGE_INTEGER   AverageWrite;
    DWORD           Granularity;
    DWORD           Size;
    DWORD           ReadCount;
    DWORD           WriteCount;
    PHISTOGRAM_BUCKET  Histogram;
} DISK_HISTOGRAM, *PDISK_HISTOGRAM;

typedef struct _DISK_PERFORMANCE {
        LARGE_INTEGER BytesRead;
        LARGE_INTEGER BytesWritten;
        LARGE_INTEGER ReadTime;
        LARGE_INTEGER WriteTime;
        LARGE_INTEGER IdleTime;
        DWORD ReadCount;
        DWORD WriteCount;
        DWORD QueueDepth;
        DWORD SplitCount;
        LARGE_INTEGER QueryTime;
        DWORD   StorageDeviceNumber;
        WCHAR   StorageManagerName[8];
} DISK_PERFORMANCE, *PDISK_PERFORMANCE;

typedef struct _DISK_RECORD {
   LARGE_INTEGER ByteOffset;
   LARGE_INTEGER StartTime;
   LARGE_INTEGER EndTime;
   PVOID VirtualAddress;
   DWORD NumberOfBytes;
   BYTE  DeviceNumber;
   BOOLEAN ReadRequest;
} DISK_RECORD, *PDISK_RECORD;

typedef struct _DISK_LOGGING {
    BYTE  Function;
    PVOID BufferAddress;
    DWORD BufferSize;
} DISK_LOGGING, *PDISK_LOGGING;


typedef enum _BIN_TYPES {
    RequestSize,
    RequestLocation
} BIN_TYPES;

typedef struct _BIN_RANGE {
    LARGE_INTEGER StartValue;
    LARGE_INTEGER Length;
} BIN_RANGE, *PBIN_RANGE;

typedef struct _PERF_BIN {
    DWORD NumberOfBins;
    DWORD TypeOfBin;
    BIN_RANGE BinsRanges[1];
} PERF_BIN, *PPERF_BIN ;

typedef struct _BIN_COUNT {
    BIN_RANGE BinRange;
    DWORD BinCount;
} BIN_COUNT, *PBIN_COUNT;

typedef struct _BIN_RESULTS {
    DWORD NumberOfBins;
    BIN_COUNT BinCounts[1];
} BIN_RESULTS, *PBIN_RESULTS;

#pragma pack(1)
typedef struct _GETVERSIONINPARAMS {
        BYTE     bVersion;               
        BYTE     bRevision;              
        BYTE     bReserved;              
        BYTE     bIDEDeviceMap;          
        DWORD   fCapabilities;          
        DWORD   dwReserved[4];          
} GETVERSIONINPARAMS, *PGETVERSIONINPARAMS, *LPGETVERSIONINPARAMS;

typedef struct _IDEREGS {
        BYTE     bFeaturesReg;           
        BYTE     bSectorCountReg;        
        BYTE     bSectorNumberReg;       
        BYTE     bCylLowReg;             
        BYTE     bCylHighReg;            
        BYTE     bDriveHeadReg;          
        BYTE     bCommandReg;            
        BYTE     bReserved;                      
} IDEREGS, *PIDEREGS, *LPIDEREGS;

typedef struct _SENDCMDINPARAMS {
        DWORD   cBufferSize;            
        IDEREGS irDriveRegs;            
        BYTE     bDriveNumber;           
                                                                
        BYTE     bReserved[3];           
        DWORD   dwReserved[4];          
        BYTE     bBuffer[1];                     
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

typedef struct _DRIVERSTATUS {
        BYTE     bDriverError;           
                                                                
        BYTE     bIDEError;                      
                                                                
                                                                
        BYTE     bReserved[2];           
        DWORD   dwReserved[2];          
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

typedef struct _SENDCMDOUTPARAMS {
        DWORD                   cBufferSize;            
        DRIVERSTATUS            DriverStatus;           
        BYTE                    bBuffer[1];             
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;
#pragma pack()

typedef  enum _ELEMENT_TYPE {
    AllElements,        
    ChangerTransport,   
    ChangerSlot,        
    ChangerIEPort,      
    ChangerDrive,       
    ChangerDoor,        
    ChangerKeypad,      
    ChangerMaxElement   
} ELEMENT_TYPE, *PELEMENT_TYPE;

typedef  struct _CHANGER_ELEMENT {
    ELEMENT_TYPE    ElementType;
    DWORD   ElementAddress;
} CHANGER_ELEMENT, *PCHANGER_ELEMENT;

typedef  struct _CHANGER_ELEMENT_LIST {
    CHANGER_ELEMENT Element;
    DWORD   NumberOfElements;
} CHANGER_ELEMENT_LIST , *PCHANGER_ELEMENT_LIST;

typedef  struct _GET_CHANGER_PARAMETERS {
    DWORD Size;
    WORD   NumberTransportElements;
    WORD   NumberStorageElements;                
    WORD   NumberCleanerSlots;                   
    WORD   NumberIEElements;
    WORD   NumberDataTransferElements;
    WORD   NumberOfDoors;
    WORD   FirstSlotNumber;
    WORD   FirstDriveNumber;
    WORD   FirstTransportNumber;
    WORD   FirstIEPortNumber;
    WORD   FirstCleanerSlotAddress;
    WORD   MagazineSize;
    DWORD DriveCleanTimeout;
    DWORD Features0;
    DWORD Features1;
    BYTE  MoveFromTransport;
    BYTE  MoveFromSlot;
    BYTE  MoveFromIePort;
    BYTE  MoveFromDrive;
    BYTE  ExchangeFromTransport;
    BYTE  ExchangeFromSlot;
    BYTE  ExchangeFromIePort;
    BYTE  ExchangeFromDrive;
    BYTE  LockUnlockCapabilities;
    BYTE  PositionCapabilities;
    BYTE  Reserved1[2];
    DWORD Reserved2[2];

} GET_CHANGER_PARAMETERS, * PGET_CHANGER_PARAMETERS;

typedef  struct _CHANGER_PRODUCT_DATA {
    BYTE  VendorId[VENDOR_ID_LENGTH];
    BYTE  ProductId[PRODUCT_ID_LENGTH];
    BYTE  Revision[REVISION_LENGTH];
    BYTE  SerialNumber[SERIAL_NUMBER_LENGTH];
    BYTE  DeviceType;

} CHANGER_PRODUCT_DATA, *PCHANGER_PRODUCT_DATA;

typedef struct _CHANGER_SET_ACCESS {
    CHANGER_ELEMENT Element;
    DWORD           Control;
} CHANGER_SET_ACCESS, *PCHANGER_SET_ACCESS;

typedef struct _CHANGER_READ_ELEMENT_STATUS {
    CHANGER_ELEMENT_LIST ElementList;
    BOOLEAN VolumeTagInfo;
} CHANGER_READ_ELEMENT_STATUS, *PCHANGER_READ_ELEMENT_STATUS;

typedef  struct _CHANGER_ELEMENT_STATUS {
    CHANGER_ELEMENT Element;
    CHANGER_ELEMENT SrcElementAddress;
    DWORD Flags;
    DWORD ExceptionCode;
    BYTE  TargetId;
    BYTE  Lun;
    WORD   Reserved;
    BYTE  PrimaryVolumeID[MAX_VOLUME_ID_SIZE];
    BYTE  AlternateVolumeID[MAX_VOLUME_ID_SIZE];
} CHANGER_ELEMENT_STATUS, *PCHANGER_ELEMENT_STATUS;

typedef struct _CHANGER_INITIALIZE_ELEMENT_STATUS {
    CHANGER_ELEMENT_LIST ElementList;
    BOOLEAN BarCodeScan;
} CHANGER_INITIALIZE_ELEMENT_STATUS, *PCHANGER_INITIALIZE_ELEMENT_STATUS;

typedef struct _CHANGER_SET_POSITION {
    CHANGER_ELEMENT Transport;
    CHANGER_ELEMENT Destination;
    BOOLEAN         Flip;
} CHANGER_SET_POSITION, *PCHANGER_SET_POSITION;

typedef struct _CHANGER_EXCHANGE_MEDIUM {
    CHANGER_ELEMENT Transport;
    CHANGER_ELEMENT Source;
    CHANGER_ELEMENT Destination1;
    CHANGER_ELEMENT Destination2;
    BOOLEAN         Flip1;
    BOOLEAN         Flip2;
} CHANGER_EXCHANGE_MEDIUM, *PCHANGER_EXCHANGE_MEDIUM;

typedef struct _CHANGER_MOVE_MEDIUM {
    CHANGER_ELEMENT Transport;
    CHANGER_ELEMENT Source;
    CHANGER_ELEMENT Destination;
    BOOLEAN         Flip;
} CHANGER_MOVE_MEDIUM, *PCHANGER_MOVE_MEDIUM;

typedef  struct _CHANGER_SEND_VOLUME_TAG_INFORMATION {
    CHANGER_ELEMENT StartingElement;
    DWORD ActionCode;
    BYTE  VolumeIDTemplate[MAX_VOLUME_TEMPLATE_SIZE];
} CHANGER_SEND_VOLUME_TAG_INFORMATION, *PCHANGER_SEND_VOLUME_TAG_INFORMATION;

typedef struct _READ_ELEMENT_ADDRESS_INFO {
    DWORD NumberOfElements;
    CHANGER_ELEMENT_STATUS ElementStatus[1];
} READ_ELEMENT_ADDRESS_INFO, *PREAD_ELEMENT_ADDRESS_INFO;


#ifndef _FILESYSTEMFSCTL_
#define _FILESYSTEMFSCTL_
typedef struct _PATHNAME_BUFFER {

    DWORD PathNameLength;
    WCHAR Name[1];

} PATHNAME_BUFFER, *PPATHNAME_BUFFER;

typedef struct _FSCTL_QUERY_FAT_BPB_BUFFER {

    BYTE  First0x24BytesOfBootSector[0x24];

} FSCTL_QUERY_FAT_BPB_BUFFER, *PFSCTL_QUERY_FAT_BPB_BUFFER;

typedef struct {

    LARGE_INTEGER VolumeSerialNumber;
    LARGE_INTEGER NumberSectors;
    LARGE_INTEGER TotalClusters;
    LARGE_INTEGER FreeClusters;
    LARGE_INTEGER TotalReserved;
    DWORD BytesPerSector;
    DWORD BytesPerCluster;
    DWORD BytesPerFileRecordSegment;
    DWORD ClustersPerFileRecordSegment;
    LARGE_INTEGER MftValidDataLength;
    LARGE_INTEGER MftStartLcn;
    LARGE_INTEGER Mft2StartLcn;
    LARGE_INTEGER MftZoneStart;
    LARGE_INTEGER MftZoneEnd;

} NTFS_VOLUME_DATA_BUFFER, *PNTFS_VOLUME_DATA_BUFFER;

typedef struct {

    DWORD ByteCount;

    WORD   MajorVersion;
    WORD   MinorVersion;

} NTFS_EXTENDED_VOLUME_DATA, *PNTFS_EXTENDED_VOLUME_DATA;

typedef struct {

    LARGE_INTEGER StartingLcn;

} STARTING_LCN_INPUT_BUFFER, *PSTARTING_LCN_INPUT_BUFFER;

typedef struct {

    LARGE_INTEGER StartingLcn;
    LARGE_INTEGER BitmapSize;
    BYTE  Buffer[1];

} VOLUME_BITMAP_BUFFER, *PVOLUME_BITMAP_BUFFER;

typedef struct {

    LARGE_INTEGER StartingVcn;

} STARTING_VCN_INPUT_BUFFER, *PSTARTING_VCN_INPUT_BUFFER;

typedef struct RETRIEVAL_POINTERS_BUFFER {

    DWORD ExtentCount;
    LARGE_INTEGER StartingVcn;
    struct {
        LARGE_INTEGER NextVcn;
        LARGE_INTEGER Lcn;
    } Extents[1];

} RETRIEVAL_POINTERS_BUFFER, *PRETRIEVAL_POINTERS_BUFFER;

typedef struct {

    LARGE_INTEGER FileReferenceNumber;

} NTFS_FILE_RECORD_INPUT_BUFFER, *PNTFS_FILE_RECORD_INPUT_BUFFER;

typedef struct {

    LARGE_INTEGER FileReferenceNumber;
    DWORD FileRecordLength;
    BYTE  FileRecordBuffer[1];

} NTFS_FILE_RECORD_OUTPUT_BUFFER, *PNTFS_FILE_RECORD_OUTPUT_BUFFER;

typedef struct {

    HANDLE FileHandle;
    LARGE_INTEGER StartingVcn;
    LARGE_INTEGER StartingLcn;
    DWORD ClusterCount;

} MOVE_FILE_DATA, *PMOVE_FILE_DATA;

typedef struct {

    BYTE  FinderInfo[32];

} HFS_INFORMATION_BUFFER, *PHFS_INFORMATION_BUFFER;

typedef struct {
    DWORD Restart;
    SID Sid;
} FIND_BY_SID_DATA, *PFIND_BY_SID_DATA;

typedef struct {

    DWORDLONG StartFileReferenceNumber;
    USN LowUsn;
    USN HighUsn;

} MFT_ENUM_DATA, *PMFT_ENUM_DATA;

typedef struct {

    DWORDLONG MaximumSize;
    DWORDLONG AllocationDelta;

} CREATE_USN_JOURNAL_DATA, *PCREATE_USN_JOURNAL_DATA;

typedef struct {

    USN StartUsn;
    DWORD ReasonMask;
    DWORD ReturnOnlyOnClose;
    DWORDLONG Timeout;
    DWORDLONG BytesToWaitFor;
    DWORDLONG UsnJournalID;

} READ_USN_JOURNAL_DATA, *PREAD_USN_JOURNAL_DATA;

typedef struct {

    DWORD RecordLength;
    WORD   MajorVersion;
    WORD   MinorVersion;
    DWORDLONG FileReferenceNumber;
    DWORDLONG ParentFileReferenceNumber;
    USN Usn;
    LARGE_INTEGER TimeStamp;
    DWORD Reason;
    DWORD SourceInfo;
    DWORD SecurityId;
    DWORD FileAttributes;
    WORD   FileNameLength;
    WORD   FileNameOffset;
    WCHAR FileName[1];

} USN_RECORD, *PUSN_RECORD;

typedef struct {

    DWORDLONG UsnJournalID;
    USN FirstUsn;
    USN NextUsn;
    USN LowestValidUsn;
    USN MaxUsn;
    DWORDLONG MaximumSize;
    DWORDLONG AllocationDelta;

} USN_JOURNAL_DATA, *PUSN_JOURNAL_DATA;

typedef struct {

    DWORDLONG UsnJournalID;
    DWORD DeleteFlags;

} DELETE_USN_JOURNAL_DATA, *PDELETE_USN_JOURNAL_DATA;

typedef struct {

    DWORD UsnSourceInfo;
    HANDLE VolumeHandle;
    DWORD HandleInfo;

} MARK_HANDLE_INFO, *PMARK_HANDLE_INFO;

typedef struct {

    ACCESS_MASK DesiredAccess;
    DWORD SecurityIds[1];

} BULK_SECURITY_TEST_DATA, *PBULK_SECURITY_TEST_DATA;

typedef struct _FILESYSTEM_STATISTICS {

    WORD   FileSystemType;
    WORD   Version;                     

    DWORD SizeOfCompleteStructure;      

    DWORD UserFileReads;
    DWORD UserFileReadBytes;
    DWORD UserDiskReads;
    DWORD UserFileWrites;
    DWORD UserFileWriteBytes;
    DWORD UserDiskWrites;

    DWORD MetaDataReads;
    DWORD MetaDataReadBytes;
    DWORD MetaDataDiskReads;
    DWORD MetaDataWrites;
    DWORD MetaDataWriteBytes;
    DWORD MetaDataDiskWrites;

} FILESYSTEM_STATISTICS, *PFILESYSTEM_STATISTICS;

typedef struct _FAT_STATISTICS {
    DWORD CreateHits;
    DWORD SuccessfulCreates;
    DWORD FailedCreates;

    DWORD NonCachedReads;
    DWORD NonCachedReadBytes;
    DWORD NonCachedWrites;
    DWORD NonCachedWriteBytes;

    DWORD NonCachedDiskReads;
    DWORD NonCachedDiskWrites;
} FAT_STATISTICS, *PFAT_STATISTICS;

typedef struct _NTFS_STATISTICS {

    DWORD LogFileFullExceptions;
    DWORD OtherExceptions;
    DWORD MftReads;
    DWORD MftReadBytes;
    DWORD MftWrites;
    DWORD MftWriteBytes;
    struct {
        WORD   Write;
        WORD   Create;
        WORD   SetInfo;
        WORD   Flush;
    } MftWritesUserLevel;

    WORD   MftWritesFlushForLogFileFull;
    WORD   MftWritesLazyWriter;
    WORD   MftWritesUserRequest;

    DWORD Mft2Writes;
    DWORD Mft2WriteBytes;
    struct {
        WORD   Write;
        WORD   Create;
        WORD   SetInfo;
        WORD   Flush;
    } Mft2WritesUserLevel;

    WORD   Mft2WritesFlushForLogFileFull;
    WORD   Mft2WritesLazyWriter;
    WORD   Mft2WritesUserRequest;

    DWORD RootIndexReads;
    DWORD RootIndexReadBytes;
    DWORD RootIndexWrites;
    DWORD RootIndexWriteBytes;

    DWORD BitmapReads;
    DWORD BitmapReadBytes;
    DWORD BitmapWrites;
    DWORD BitmapWriteBytes;

    WORD   BitmapWritesFlushForLogFileFull;
    WORD   BitmapWritesLazyWriter;
    WORD   BitmapWritesUserRequest;

    struct {
        WORD   Write;
        WORD   Create;
        WORD   SetInfo;
    } BitmapWritesUserLevel;

    DWORD MftBitmapReads;
    DWORD MftBitmapReadBytes;
    DWORD MftBitmapWrites;
    DWORD MftBitmapWriteBytes;

    WORD   MftBitmapWritesFlushForLogFileFull;
    WORD   MftBitmapWritesLazyWriter;
    WORD   MftBitmapWritesUserRequest;

    struct {
        WORD   Write;
        WORD   Create;
        WORD   SetInfo;
        WORD   Flush;
    } MftBitmapWritesUserLevel;

    DWORD UserIndexReads;
    DWORD UserIndexReadBytes;
    DWORD UserIndexWrites;
    DWORD UserIndexWriteBytes;

    DWORD LogFileReads;
    DWORD LogFileReadBytes;
    DWORD LogFileWrites;
    DWORD LogFileWriteBytes;

    struct {
        DWORD Calls;                
        DWORD Clusters;             
        DWORD Hints;                

        DWORD RunsReturned;         

        DWORD HintsHonored;         
        DWORD HintsClusters;        
        DWORD Cache;                
        DWORD CacheClusters;        
        DWORD CacheMiss;            
        DWORD CacheMissClusters;    
    } Allocate;

} NTFS_STATISTICS, *PNTFS_STATISTICS;

typedef struct _FILE_OBJECTID_BUFFER {
    BYTE  ObjectId[16];
    union {
        struct {
            BYTE  BirthVolumeId[16];
            BYTE  BirthObjectId[16];
            BYTE  DomainId[16];
        } ;
        BYTE  ExtendedInfo[48];
    };

} FILE_OBJECTID_BUFFER, *PFILE_OBJECTID_BUFFER;

typedef struct _FILE_SET_SPARSE_BUFFER {
    BOOLEAN SetSparse;
} FILE_SET_SPARSE_BUFFER, *PFILE_SET_SPARSE_BUFFER;

typedef struct _FILE_ZERO_DATA_INFORMATION {

    LARGE_INTEGER FileOffset;
    LARGE_INTEGER BeyondFinalZero;

} FILE_ZERO_DATA_INFORMATION, *PFILE_ZERO_DATA_INFORMATION;

typedef struct _FILE_ALLOCATED_RANGE_BUFFER {

    LARGE_INTEGER FileOffset;
    LARGE_INTEGER Length;

} FILE_ALLOCATED_RANGE_BUFFER, *PFILE_ALLOCATED_RANGE_BUFFER;

typedef struct _ENCRYPTION_BUFFER {

    DWORD EncryptionOperation;
    BYTE  Private[1];

} ENCRYPTION_BUFFER, *PENCRYPTION_BUFFER;

typedef struct _DECRYPTION_STATUS_BUFFER {

    BOOLEAN NoEncryptedStreams;

} DECRYPTION_STATUS_BUFFER, *PDECRYPTION_STATUS_BUFFER;

typedef struct _REQUEST_RAW_ENCRYPTED_DATA {
    LONGLONG FileOffset;
    DWORD Length;

} REQUEST_RAW_ENCRYPTED_DATA, *PREQUEST_RAW_ENCRYPTED_DATA;

typedef struct _ENCRYPTED_DATA_INFO {

    DWORDLONG StartingFileOffset;
    DWORD OutputBufferOffset;
    DWORD BytesWithinFileSize;
    DWORD BytesWithinValidDataLength;
    WORD   CompressionFormat;
    BYTE  DataUnitShift;
    BYTE  ChunkShift;
    BYTE  ClusterShift;
    BYTE  EncryptionFormat;
    WORD   NumberOfDataBlocks;
    DWORD DataBlockSize[ANYSIZE_ARRAY];

} ENCRYPTED_DATA_INFO;

typedef ENCRYPTED_DATA_INFO *PENCRYPTED_DATA_INFO;

typedef struct _SI_COPYFILE {
    DWORD SourceFileNameLength;
    DWORD DestinationFileNameLength;
    DWORD Flags;
    WCHAR FileNameBuffer[1];
} SI_COPYFILE, *PSI_COPYFILE;

typedef struct _NSS_CONTROL {
    DWORD   code;
    DWORD   param;
} NSS_CONTROL, *PNSS_CONTROL;

#endif 

typedef struct _DISK_EXTENT {
    DWORD           DiskNumber;
    LARGE_INTEGER   StartingOffset;
    LARGE_INTEGER   ExtentLength;
} DISK_EXTENT, *PDISK_EXTENT;

typedef struct _VOLUME_DISK_EXTENTS {
    DWORD       NumberOfDiskExtents;
    DISK_EXTENT Extents[1];
} VOLUME_DISK_EXTENTS, *PVOLUME_DISK_EXTENTS;

#pragma pack()