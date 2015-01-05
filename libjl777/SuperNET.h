//
//  SuperNET.h
//  libtest
//
//  Created by jl777 on 8/13/14.
//  Copyright (c) 2014 jl777. MIT License.
//

#ifndef libtest_libjl777_h
#define libtest_libjl777_h
#define HUFF_GENMODE

#include <stdint.h>
#include "uthash.h"

#define MAX_PUBADDR_TIME (24 * 60 * 60)
#define PUBLIC_DATA 0
#define PRIVATE_DATA 1
#define TELEPOD_DATA 2
#define DEADDROP_DATA 3
#define CONTACT_DATA 4
#define NODESTATS_DATA 5
#define INSTANTDEX_DATA 6
#define MULTISIG_DATA 7
#define ADDRESS_DATA 8
#define PRICE_DATA 9
#define NUM_SUPERNET_DBS (PRICE_DATA+1)
#define SMALLVAL .000000000000001
#define _SUPERNET_PORT 7777

#define MAX_COINTXID_LEN 128
#define MAX_COINADDR_LEN 128
#define MAX_NXT_STRLEN 24
#define MAX_NXTTXID_LEN MAX_NXT_STRLEN
#define MAX_NXTADDR_LEN MAX_NXT_STRLEN
#define MAX_TRANSFER_SIZE (65536 * 16)
#define TRANSFER_BLOCKSIZE 512
#define MAX_TRANSFER_BLOCKS (MAX_TRANSFER_SIZE / TRANSFER_BLOCKSIZE)

static uint8_t huffmasks[8] = { (1<<0), (1<<1), (1<<2), (1<<3), (1<<4), (1<<5), (1<<6), (1<<7) };
static uint8_t huffoppomasks[8] = { ~(1<<0), ~(1<<1), ~(1<<2), ~(1<<3), ~(1<<4), ~(1<<5), ~(1<<6), (uint8_t)~(1<<7) };
struct huffstream { uint8_t *ptr,*buf; int32_t bitoffset,maski,endpos,allocsize; };
typedef struct huffstream HUFF;

void hclose(HUFF *hp);
HUFF *hopen(uint8_t *bits,int32_t num);
void hrewind(HUFF *hp);
void hclear(HUFF *hp);
int32_t hseek(HUFF *hp,int32_t offset,int32_t mode);
int32_t hgetbit(HUFF *hp);
int32_t hputbit(HUFF *hp,int32_t bit);
int32_t hwrite(uint64_t codebits,int32_t numbits,HUFF *hp);
int32_t hflush(FILE *fp,HUFF *hp);

union _bits256 { uint8_t bytes[32]; uint16_t ushorts[16]; uint32_t uints[8]; uint64_t ulongs[4]; uint64_t txid; };
union _bits64 { uint8_t bytes[8]; uint16_t ushorts[4]; uint32_t uints[2]; uint64_t txid; };
typedef union _bits64 bits64;
typedef union _bits256 bits256;

union hufftype
{
    bits64 bits;
    void *str;
};

#ifdef HUFF_GENMODE
#define HUFF_NUMFREQS 1
#define HUFF_READONLY 0
#else
#define HUFF_READONLY 1
#define HUFF_NUMFREQS 10
#endif


struct hashtable
{
    char *name;//[128];
    void **hashtable;
    uint64_t hashsize,numsearches,numiterations,numitems;
    long keyoffset,keysize,modifiedoffset,structsize;
};

#define BITSTREAM_UNIQUE (1<<0)
#define BITSTREAM_STRING (1<<1)
#define BITSTREAM_HEXSTR (1<<2)
#define BITSTREAM_COMPRESSED (1<<3)
#define BITSTREAM_STATSONLY (1<<4)
#define BITSTREAM_VALUE (1<<5)
#define BITSTREAM_SCRIPT (1<<6)
#define BITSTREAM_VINS (1<<7)
#define BITSTREAM_VOUTS (1<<8)
#define BITSTREAM_32bits (1<<9)
#define BITSTREAM_64bits (1<<10)

struct huffcode
{
    const struct huffitem **items;
    int32_t numinds,maxbits,numnodes,depth,maxind,*tree;
    double totalbits,totalbytes;
};

struct voutinfo { uint32_t tp_ind,vout,addr_ind,sp_ind; uint64_t value; };
struct address_entry { uint64_t blocknum:32,txind:15,vinflag:1,v:14,spent:1,isinternal:1; };
struct blockinfo { uint32_t firstvout,firstvin; };

struct valueinfo { uint64_t amount; };
struct scriptinfo { uint32_t addrind; char mode; };
struct txinfo { uint16_t txind,numvouts,numvins; };
union huffinfo
{
    uint8_t c; uint16_t s; uint32_t i; void *ptr;
    struct valueinfo value;
    struct scriptinfo script;
    struct txinfo tx;
};

struct huffitem
{
    union huffinfo U;
    UT_hash_handle hh;
    uint64_t codebits;
    uint32_t huffind,fpos,freq[HUFF_NUMFREQS];
    uint16_t size:5,numbits:11;
    uint8_t isptr,wt;
    char str[];
};

struct bitstream_file
{
    struct huffitem *dataptr,**itemptrs;
    FILE *fp;
    long itemsize;
    char fname[1024],coinstr[16],typestr[16],stringflag;
    uint32_t blocknum,ind,checkblock,refblock,mode,huffid,huffwt,maxitems,nomemstructs;
};


struct compressionvars
{
    HUFF *hp;
    struct blockinfo prevB;
    uint32_t valuebfp,addrbfp,txidbfp,scriptbfp,voutsbfp,vinsbfp,bitstream,numbfps;
    uint32_t maxitems,maxblocknum,firstblock,blocknum,processed,firstvout,firstvin;
    uint8_t *buffer,*rawbits;
    struct bitstream_file *bfps[16],*numvoutsbfp,*numvinsbfp,*inblockbfp,*txinbfp,*invoutbfp;
    char *disp,coinstr[64];
    double startmilli;
};

struct transfer_args
{
    uint64_t modified;
    uint32_t timestamps[MAX_TRANSFER_BLOCKS],crcs[MAX_TRANSFER_BLOCKS],gotcrcs[MAX_TRANSFER_BLOCKS],slots[MAX_TRANSFER_BLOCKS];
    char previpaddr[64],sender[64],dest[64],name[512],hashstr[65],handler[64],pstr[MAX_TRANSFER_BLOCKS];
    uint8_t data[MAX_TRANSFER_SIZE],snapshot[MAX_TRANSFER_SIZE];
    uint32_t totalcrc,currentcrc,snapshotcrc,handlercrc,syncmem,totallen,blocksize,numfrags,completed,timeout,handlertime;
};

struct storage_header
{
    uint32_t size,createtime;
    uint64_t keyhash;
};

struct SuperNET_storage // for public and private data
{
    struct storage_header H;
    unsigned char data[];
};

struct telepod
{
    struct storage_header H;
    int32_t vout;
    uint32_t crc,pad2,clonetime,cloneout,podstate,inhwm,pad;
    double evolve_amount;
    char clonetxid[MAX_COINTXID_LEN],cloneaddr[MAX_COINADDR_LEN];
    uint64_t senderbits,destbits,unspent,modified,satoshis; // everything after modified is used for crc
    char coinstr[8],txid[MAX_COINTXID_LEN],coinaddr[MAX_COINADDR_LEN],script[128];
    char privkey[];
};

struct pserver_info
{
    char ipaddr[64];
    uint64_t modified,nxt64bits;
    float recvmilli,sentmilli;
    void *udps[2];
    uint32_t decrypterrs,lastcontact,numsent,numrecv;
    uint16_t p2pport,firstport,lastport,supernet_port,supernet_altport;
};

struct nodestats
{
    struct storage_header H;
    uint8_t pubkey[256>>3];
    struct nodestats *eviction;
    uint64_t nxt64bits,coins[4];
    double pingpongsum;
    float pingmilli,pongmilli;
    uint32_t ipbits,lastcontact,numpings,numpongs;
    uint8_t BTCD_p2p,gotencrypted,modified,expired,isMM;
};

struct contact_info
{
    struct storage_header H;
    bits256 pubkey,shared;
    char handle[64];
    uint64_t nxt64bits,deaddrop,mydrop;
    int32_t numsent,numrecv,lastrecv,lastsent,lastentry;
    char jsonstr[32768];
};

struct InstantDEX_quote { uint64_t nxt64bits,baseamount,relamount; uint32_t timestamp,type; };

struct orderbook_info { uint64_t baseid,relid,obookid; };

struct exchange_pair { struct storage_header H; char exchange[64],base[16],rel[16]; };

struct pubkey_info { uint64_t nxt64bits; uint32_t ipbits; char pubkey[256],coinaddr[128]; };

struct multisig_addr
{
    struct storage_header H;
    char NXTaddr[MAX_NXTADDR_LEN],multisigaddr[MAX_COINADDR_LEN],NXTpubkey[96],redeemScript[2048],coinstr[16],email[128];
    uint64_t sender,modified;
    uint32_t m,n,created,valid,buyNXT;
    struct pubkey_info pubkeys[];
};


struct storage_header **copy_all_DBentries(int32_t *nump,int32_t selector);

void init_jl777(char *myip);
int SuperNET_start(char *JSON_or_fname,char *myip);
char *SuperNET_JSON(char *JSONstr);
char *SuperNET_gotpacket(char *msg,int32_t duration,char *from_ip_port);
int32_t SuperNET_broadcast(char *msg,int32_t duration);
int32_t SuperNET_narrowcast(char *destip,unsigned char *msg,int32_t len);
int32_t got_newpeer(const char *ip_port);

void *portable_thread_create(void *funcp,void *argp);

#endif

