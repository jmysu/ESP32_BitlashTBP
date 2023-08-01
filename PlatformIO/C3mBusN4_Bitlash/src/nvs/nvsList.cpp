// Show_nvs_keys.ino
// Read all the keys from nvs partition and dump this information.
// 04-dec-2017, Ed Smallenburg.
//
#include <Arduino.h>

#include <stdio.h>
#include <string.h>
#include <esp_partition.h>
#include <nvs.h>

// Debug buffer size
#define DEBUG_BUFFER_SIZE 130
#define DEBUG true

struct nvs_entry
{
  uint8_t  Ns ;         // Namespace ID
  uint8_t  Type ;       // Type of value
  uint8_t  Span ;       // Number of entries used for this item
  uint8_t  Rvs ;        // Reserved, should be 0xFF
  uint32_t CRC ;        // CRC
  char     Key[16] ;    // Key in Ascii
  uint64_t Data ;       // Data in entry 
} ;

struct nvs_page                                     // For nvs entries
{                                                   // 1 page is 4096 bytes
  uint32_t  State ;
  uint32_t  Seqnr ;
  
  uint32_t  Unused[5] ;
  uint32_t  CRC ;
  uint8_t   Bitmap[32] ;
  nvs_entry Entry[126] ;
} ;

// Common data
nvs_page    bufNVS ;

//**************************************************************************************************
//                                          D B G P R I N T                                        *
//**************************************************************************************************
// Send a line of info to serial output.  Works like vsprintf(), but checks the DEBUG flag.        *
// Print only if DEBUG flag is true.  Always returns the formatted string.                         *
//**************************************************************************************************
char* dbgprint ( const char* format, ... )
{
  static char sbuf[DEBUG_BUFFER_SIZE] ;                // For debug lines
  va_list varArgs ;                                    // For variable number of params

  va_start ( varArgs, format ) ;                       // Prepare parameters
  vsnprintf ( sbuf, sizeof(sbuf), format, varArgs ) ;  // Format the message
  va_end ( varArgs ) ;                                 // End of using parameters
  if ( DEBUG )                                         // DEBUG on?
  {
    Serial.print ( "\tNVS: " ) ;                           // Yes, print prefix
    Serial.println ( sbuf ) ;                          // and the info
  }
  return sbuf ;                                        // Return stored string
}


//**************************************************************************************************
//                                   F I N D N S I D                                               *
//**************************************************************************************************
// Find the namespace ID for the namespace passed as parameter.                                    *
//**************************************************************************************************
uint8_t FindNsID ( const esp_partition_t* nvs, const char* ns )
{
  esp_err_t                 result = ESP_OK ;                 // Result of reading partition
  uint32_t                  offset = 0 ;                      // Offset in nvs partition
  uint8_t                   i ;                               // Index in Entry 0..125
  uint8_t                   bm ;                              // Bitmap for an entry
  uint8_t                   res = 0xFF ;                      // Function result


  while ( offset < nvs->size )
  {
    result = esp_partition_read ( nvs, offset,                // Read 1 page in nvs partition
                                  &bufNVS,
                                  sizeof(nvs_page) ) ;
    if ( result != ESP_OK )
    {
      dbgprint ( "Error reading NVS!" ) ;
      break ;
    }
    i = 0 ;
    while ( i < 126 )
    {
      bm = ( bufNVS.Bitmap[i/4] >> ( ( i % 4 ) * 2 ) ) & 0x03 ;  // Get bitmap for this entry
      if ( ( bm == 2 ) &&
           ( bufNVS.Entry[i].Ns == 0 ) &&  
           ( strcmp ( ns, bufNVS.Entry[i].Key ) == 0 ) )
      {
        res = bufNVS.Entry[i].Data & 0xFF ;                      // Return the ID
        offset = nvs->size ;                                  // Stop outer loop as well
        break ;
      }
      else
      {
        if ( bm == 2 )
        {
          i += bufNVS.Entry[i].Span ;                             // Next entry
        }
        else
        {
          i++ ;
        }
      }
    }
    offset += sizeof(nvs_page) ;                              // Prepare to read next page in nvs
  }
  return res ;
}

String nvsDataType(int type)
{
  switch (type){
    case 0: return "U8"; break;
    case 1: return "I8"; break;
    /*
    U16  = NVS_TYPE_U16,
    I16  = NVS_TYPE_I16,
    U32  = NVS_TYPE_U32,
    I32  = NVS_TYPE_I32,
    U64  = NVS_TYPE_U64,
    I64  = NVS_TYPE_I64,
    SZ   = NVS_TYPE_STR,
    */
    case 66: return "data[]";break;
    case 72: return "idx[]";break;
    default: return "any";break;
    }
}

//**************************************************************************************************
//                                          S E T U P                                              *
//**************************************************************************************************
//**************************************************************************************************
void nvsList()
{
  esp_partition_iterator_t  pi ;                              // Iterator for find
  const esp_partition_t*    nvs ;                             // Pointer to partition struct
  esp_err_t                 result = ESP_OK ;
  const char*               partname = "nvs" ;
  uint8_t                   pagenr = 0 ;                      // Page number in NVS
  uint8_t                   i ;                               // Index in Entry 0..125
  uint8_t                   bm ;                              // Bitmap for an entry
  uint32_t                  offset = 0 ;                      // Offset in nvs partition
  uint8_t                   namespace_ID ;                    // Namespace ID found
  
  //Serial.begin ( 115200 ) ;                                   // For debug
  //Serial.println() ;


  pi = esp_partition_find ( ESP_PARTITION_TYPE_DATA,          // Get partition iterator for
                            ESP_PARTITION_SUBTYPE_ANY,        // this partition
                            partname ) ;
  if ( pi )  {
    nvs = esp_partition_get ( pi ) ;                          // Get partition struct
    esp_partition_iterator_release ( pi ) ;                   // Release the iterator
    dbgprint ( "Partition \"%s\" found %d KB",
               partname,
               nvs->size/1024 ) ;
  }
  else  {
    dbgprint ( "Partition %s not found!", partname ) ;
    return ;
  }

  //namespace_ID = FindNsID ( nvs, "myprefs" ) ;             // Find ID of our namespace in NVS
  //dbgprint ( "Namespace ID of [myprefs] is %d", namespace_ID ) ;
  namespace_ID =0xFF;

  while ( offset < nvs->size ){
    result = esp_partition_read ( nvs, offset,                // Read 1 page in nvs partition
                                  &bufNVS,
                                  sizeof(nvs_page) ) ;
    if ( result != ESP_OK ){
      dbgprint ( "Error reading NVS!" ) ;
      return ;
      }
    //dbgprint ( "" ) ;
    //dbgprint ( "Dump page %d",  pagenr ) ;
    //dbgprint ( "State is %08X", bufNVS.State ) ;
    //dbgprint ( "Seqnr is %08X", bufNVS.Seqnr ) ;
    //dbgprint ( "CRC   is %08X", bufNVS.CRC ) ;
    i = 0 ;
    while ( i < 126 ){
      bm = ( bufNVS.Bitmap[i/4] >> ( ( i % 4 ) * 2 ) ) & 0x03 ;  // Get bitmap for this entry
      if ( bm == 2 ){
        if ( ( namespace_ID == 0xFF ) ||                      // Show all if ID = 0xFF
             ( bufNVS.Entry[i].Ns == namespace_ID ) )         // otherwise just my namespace
        {
          dbgprint ( "Key[%03d]: \"%s\", %s", i,               // Print entrynr
                    bufNVS.Entry[i].Key, nvsDataType(bufNVS.Entry[i].Type));                  // Print the key & type
        }
        i += bufNVS.Entry[i].Span ;                           // Next entry
      }
      else {
        i++ ;
      }
    }
    offset += sizeof(nvs_page) ;                              // Prepare to read next page in nvs
    pagenr++ ;
  }
}


void nvsPrintStates()
{
    // Example of nvs_get_stats() to get the number of used entries and free entries:
    nvs_stats_t nvs_stats;
    nvs_get_stats(NULL, &nvs_stats);
    Serial.printf("NVS Names:%d, Used/All:%d/%d(%d%%)\n",
       nvs_stats.namespace_count, nvs_stats.used_entries, nvs_stats.total_entries, (nvs_stats.used_entries*100)/nvs_stats.total_entries);

    if (nvs_stats.namespace_count) nvsList();
}
 