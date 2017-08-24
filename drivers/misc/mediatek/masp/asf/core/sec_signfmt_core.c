#include "sec_boot_lib.h"

/*                                                                         
               
                                                                          */
#define MOD                         "SFMT_CORE"

/*                                                                         
                             
                                                                          */
static int sec_signfmt_get_file_hash(char* file_name, char* hash_buf, unsigned int hash_len, 
    unsigned int offset, unsigned int size)    
{
    uint32 ret = SEC_OK;
    ASF_FILE fp = ASF_FILE_NULL;
    bool bFileOpened = false;        
    char *file_buf = NULL;
    uint32 file_size = 0;  
    bool bBufAllocate = false;        
    uint32 read_size = 0;      

    ASF_GET_DS

    /*                                     */
    /*                                     */
    /*                                     */
    if(NULL == hash_buf)
    {
        ret = ERR_FS_READ_BUF_IS_NULL;
        goto _end;
    }
    
    /*                                     */
    /*                                     */
    /*                                     */
    fp = ASF_OPEN(file_name);
    if (ASF_FILE_ERROR(fp))
    {
        ret = ERR_FS_OPEN_SEC_FILE_FAIL;
        goto _end;
    }

    SMSG(true,"\n-----------------------\n");
    SMSG(true,"[%s] read '%s'\n",MOD,file_name);
    SMSG(true,"-----------------------\n");       
    bFileOpened = true;    
    
    /*                                     */
    /*                                     */
    /*                                     */
    ASF_SEEK_END(fp, 0);
    file_size = ASF_FILE_POS(fp);
    SMSG(true,"[%s] file size '%d'\n",MOD,file_size);
    
    /*                                     */
    /*                                     */
    /*                                     */
    if(0 != size)
    {
        if( file_size > size)
        {
            file_size = size;
            SMSG(true,"[%s] adjust read size '0x%x'\n",MOD,file_size);
        }
    }
    
    /*                                     */
    /*                                     */
    /*                                     */    
    file_buf = ASF_MALLOC(file_size);
    if(NULL == file_buf)
    {   
        ret = ERR_FS_READ_BUF_ALLOCATE_FAIL;
        goto _end;
    }
    
    bBufAllocate = true;
    
    /*                                     */
    /*                                     */
    /*                                     */
    ASF_SEEK_SET(fp, offset);

    /*                                     */
    /*                                     */
    /*                                     */
    if (file_size != (read_size = ASF_READ(fp, file_buf, file_size)))
    {
        SMSG(true,"[%s] '%s' read size '%d' != '%d'\n",MOD,file_name,read_size,file_size);
        ret = ERR_FS_READ_SIZE_FAIL;
        goto _end;
    } 
    
    /*                                     */
    /*                                     */
    /*                                     */ 
    if(SEC_OK != (ret = sec_hash((uchar*)file_buf, file_size, 
            (uchar*)hash_buf, hash_len )))
    {
        SMSG(true,"[%s] generate hash fail\n\n",MOD);
        ret = ERR_SIGN_FORMAT_GENERATE_HASH_FAIL;
        goto _end;
    }
    
_end:
    
    if(true == bFileOpened)
    {
        ASF_CLOSE(fp);
    }
    
    if(true == bBufAllocate)
    {
        ASF_FREE(file_buf); 
    }

    ASF_PUT_DS

    return ret;
}
    

/*                                                                         
                           
                                                                          */
int sec_signfmt_verify_file(char *file_path, SEC_IMG_HEADER *img_hdr, unsigned int *data_offset, unsigned int *data_sec_len)
{
    unsigned int ret = SEC_OK;
    ASF_FILE fp = ASF_FILE_NULL;
    int read_sz = 0;

    ASF_GET_DS

    /*                       */
    if (NULL == file_path)
    {
        SMSG(true,"[%s] doesn't initialize fs sec file list '%s'\n",MOD,file_path);
        ret = ERR_FS_SEC_LIST_NOT_SPECIFY;
        goto _end;
    }

    /*                          */
    fp = ASF_OPEN(file_path);
    if (ASF_FILE_ERROR(fp))
    {
        SMSG(true,"[%s] open '%s' fail\n",MOD,file_path);
        ret = ERR_FS_OPEN_SEC_FILE_FAIL;
        goto _end;
    }

    /*            */
    if (SEC_IMG_HEADER_SIZE != (read_sz = ASF_READ(fp, (char*)img_hdr, SEC_IMG_HEADER_SIZE)))
    {
        SMSG(true,"[%s] '%s' read size '%d' != '%d'\n",MOD,file_path,read_sz,SEC_IMG_HEADER_SIZE);
        ret = ERR_FS_READ_SIZE_FAIL;
        goto _read_hdr_fail;
    }

    /*                   */
    if ( SEC_IMG_MAGIC != img_hdr->magic_number )
    {
        SMSG(true,"[%s] '%s' is not signed\n",MOD,file_path);
        ret = ERR_SIGN_FORMAT_MAGIC_WRONG;
        goto _not_signed;
    }
    if (is_signfmt_v4(img_hdr))
    {
        ret = sec_signfmt_verify_file_v4(fp, img_hdr);
    }
    else if (is_signfmt_v3(img_hdr))
    {
        ret = sec_signfmt_verify_file_v3(fp, img_hdr);
    }
    else
    {
        ret = sec_signfmt_verify_file_v2(fp, img_hdr);
    }

    *data_offset = SEC_IMG_HEADER_SIZE;
    *data_sec_len = SEC_IMG_HEADER_SIZE;

    if (is_signfmt_v4(img_hdr))
    {
        *data_sec_len += sec_signfmt_get_extension_length_v4(NULL, fp, img_hdr);
    }
    else if (is_signfmt_v3(img_hdr))
    {
        *data_sec_len += sec_signfmt_get_extension_length_v3(NULL, fp, img_hdr);
    }
    else
    {
        *data_sec_len += sec_signfmt_get_extension_length_v2(fp);
    }

_not_signed:    
_read_hdr_fail:
    ASF_CLOSE(fp);
_end:
    ASF_PUT_DS

    return ret;
}

/*                                                                         
                               
                                                                          */
int sec_signfmt_get_hash_length(SEC_IMG_U *img_if, ASF_FILE fp, char *ext_buf)
{
    bool v3_fmt = false;
    bool v4_fmt = false;
    SEC_IMG_HEADER img_hdr; 
    uint32 read_sz = 0;

    memset(&img_hdr, 0x00, SEC_IMG_HEADER_SIZE);
    
    /*                                                                                                                          */
    if( !ASF_FILE_ERROR(fp) )
    {
        ASF_SEEK_SET(fp, 0);
        /*            */
        if (SEC_IMG_HEADER_SIZE != (read_sz = ASF_READ(fp, (char*)&img_hdr, SEC_IMG_HEADER_SIZE)))
        {
            SMSG(true,"[%s] read size '%d' != '%d'\n",MOD,read_sz,SEC_IMG_HEADER_SIZE);
            return -1;
        }

        v3_fmt = is_signfmt_v3(&img_hdr);
        v4_fmt = is_signfmt_v4(&img_hdr);
    }
    else if(SECCFG_V3 == get_seccfg_ver())
    {
        v3_fmt = is_signfmt_v3((SEC_IMG_HEADER*)&img_if->v3.header.va);
        v4_fmt = is_signfmt_v4((SEC_IMG_HEADER*)&img_if->v3.header.va);
    }
    else
    {
        v3_fmt = is_signfmt_v3((SEC_IMG_HEADER*)&img_if->v1.header.va);
        v4_fmt = is_signfmt_v4((SEC_IMG_HEADER*)&img_if->v1.header.va);
    }
    
    if ( v4_fmt )
    {
        return sec_signfmt_get_hash_length_v4(&img_if->v3, fp, &img_hdr, ext_buf);
    }
    else if ( v3_fmt )
    {
        return sec_signfmt_get_hash_length_v3(&img_if->v3, fp, &img_hdr, ext_buf);
    }
    else
    {
        return sec_signfmt_get_hash_length_v2();
    }
}

/*                                                                         
                                    
                                                                          */
int sec_signfmt_get_signature_length(SEC_IMG_U *img_if, ASF_FILE fp, char *ext_buf)
{
    bool v3_fmt = false;
    bool v4_fmt = false;
    SEC_IMG_HEADER img_hdr; 
    uint32 read_sz = 0;

    memset(&img_hdr, 0x00, SEC_IMG_HEADER_SIZE);

    /*                                                                                                                          */
    if( !ASF_FILE_ERROR(fp) )
    {
        ASF_SEEK_SET(fp, 0);
        /*            */
        if (SEC_IMG_HEADER_SIZE != (read_sz = ASF_READ(fp, (char*)&img_hdr, SEC_IMG_HEADER_SIZE)))
        {
            SMSG(true,"[%s] read size '%d' != '%d'\n",MOD,read_sz,SEC_IMG_HEADER_SIZE);
            return -1;
        }

        v3_fmt = is_signfmt_v3(&img_hdr);
        v4_fmt = is_signfmt_v4(&img_hdr);
    }
    else if(SECCFG_V3 == get_seccfg_ver())
    {
        v3_fmt = is_signfmt_v3((SEC_IMG_HEADER*)&img_if->v3.header.va);
        v4_fmt = is_signfmt_v4((SEC_IMG_HEADER*)&img_if->v3.header.va);
    }
    else
    {
        v3_fmt = is_signfmt_v3((SEC_IMG_HEADER*)&img_if->v1.header.va);
        v4_fmt = is_signfmt_v4((SEC_IMG_HEADER*)&img_if->v1.header.va);
    }

    if ( v4_fmt )
    {
        return sec_signfmt_get_signature_length_v4(&img_if->v3, fp, &img_hdr, ext_buf);
    }
    else if ( v3_fmt )
    {
        return sec_signfmt_get_signature_length_v3(&img_if->v3, fp, &img_hdr, ext_buf);
    }
    else
    {
        return sec_signfmt_get_signature_length_v2();
    }
}


/*                                                                         
                                    
                                                                          */
int sec_signfmt_get_extension_length(SEC_IMG_U *img_if, ASF_FILE fp)
{
    bool v3_fmt = false;
    bool v4_fmt = false;
    SEC_IMG_HEADER img_hdr; 
    uint32 read_sz = 0;

    memset(&img_hdr, 0x00, SEC_IMG_HEADER_SIZE);

    /*                                                                                                                          */
    if( !ASF_FILE_ERROR(fp) )
    {
        ASF_SEEK_SET(fp, 0);
        /*            */
        if (SEC_IMG_HEADER_SIZE != (read_sz = ASF_READ(fp, (char*)&img_hdr, SEC_IMG_HEADER_SIZE)))
        {
            SMSG(true,"[%s] read size '%d' != '%d'\n",MOD,read_sz,SEC_IMG_HEADER_SIZE);
            return -1;
        }

        v3_fmt = is_signfmt_v3(&img_hdr);
        v4_fmt = is_signfmt_v3(&img_hdr);
    }
    else if(SECCFG_V3 == get_seccfg_ver())
    {
        v3_fmt = is_signfmt_v3((SEC_IMG_HEADER*)&img_if->v3.header.va);
        v4_fmt = is_signfmt_v4((SEC_IMG_HEADER*)&img_if->v3.header.va);
    }
    else
    {
        v3_fmt = is_signfmt_v3((SEC_IMG_HEADER*)&img_if->v1.header.va);
        v4_fmt = is_signfmt_v4((SEC_IMG_HEADER*)&img_if->v1.header.va);
    }
    
    if ( v4_fmt )
    {
        return sec_signfmt_get_extension_length_v4(&img_if->v3, fp, &img_hdr);
    }
    else if ( v3_fmt )
    {
        return sec_signfmt_get_extension_length_v3(&img_if->v3, fp, &img_hdr);
    }
    else
    {
        return sec_signfmt_get_extension_length_v2(fp);
    }
}


/*                                                                         
                             
                                                                          */
static int sec_signfmt_calculate_filelist_hash_core(ASF_FILE fp, char *double_hash_buf, unsigned int double_hash_len, unsigned int single_hash_len)
{
    unsigned int ret = SEC_OK; 
    uint32 i = 0;
    char *file_buf = NULL, *sec_file = NULL;     
    uint32 read_sz = 0;
    char *tmp_signal_hash_buf = NULL;
    uint32 img_content_offset = 0;
    uint32 img_content_length = 0;
    uint32 file_size = 0;


    /*              */
    memset(double_hash_buf, 0x00, double_hash_len);

    if(double_hash_len < 2*single_hash_len)
    {
        SMSG(true,"[%s] hash buffer size is invalid '%d'\n",MOD,double_hash_len);
        ret = ERR_SIGN_FORMAT_HASH_SIZE_WRONG;
        goto _end;
    }

    ASF_SEEK_END(fp, 0);
    file_size = ASF_FILE_POS(fp);

    img_content_offset = SEC_IMG_HEADER_SIZE;
    img_content_length = file_size-SEC_IMG_HEADER_SIZE-sec_signfmt_get_extension_length(NULL, fp);

    /*                      */
    file_buf = ASF_MALLOC(img_content_length);
    if(NULL == file_buf)
    {   
        ret = ERR_FS_READ_BUF_ALLOCATE_FAIL;
        goto _malloc_file_buf_fail;
    }

    /*                    */
    ASF_SEEK_SET(fp, img_content_offset);

    /*                    */
    if (img_content_length != (read_sz = ASF_READ(fp, file_buf, img_content_length)))
    {
        SMSG(true,"[%s] read sz '%d' != '%d'\n",MOD,read_sz,img_content_length);
        ret = ERR_FS_READ_SIZE_FAIL;
        goto _read_file_list_fail;
    }

    /*                                */
    tmp_signal_hash_buf = ASF_MALLOC(single_hash_len);
    if(NULL == tmp_signal_hash_buf)
    {   
        ret = ERR_FS_READ_BUF_ALLOCATE_FAIL;
        goto _malloc_hash_buf_fail;
    }

    /*                                     */
    /*                                     */
    /*                                     */        
    for(i = 0; i<img_content_length; i++)
    {            
        /*                                     */   
        if('[' == file_buf[i])
        {
            sec_file = file_buf + i + 1;
            sec_file = ASF_STRTOK(sec_file,"]");
        }
        else
        {
            continue;
        }        

        /*                                     */
        /*                                     */
        /*                                     */
        if(SEC_OK != (ret = sec_signfmt_get_file_hash(sec_file, 
            tmp_signal_hash_buf, single_hash_len, 0, 0)))
        {
            /*                   */
            if(ERR_FS_OPEN_SEC_FILE_FAIL == ret)
            {   
                SMSG(true,"[%s] file '%s' not found\n",MOD,sec_file);
                ret = SEC_OK;
                continue;
            }
            else
            {
                goto _end;
            }
        }        

        /*                                     */
        /*                                     */
        /*                                     */
        SMSG(sec_info.bMsg,"[%s] dump file hash (single):\n",MOD);
        dump_buf((uchar*)tmp_signal_hash_buf, single_hash_len);

        /*                                     */
        /*                                     */
        /*                                     */
        /*                       */
        mcpy(double_hash_buf+single_hash_len, tmp_signal_hash_buf, single_hash_len);

        memset(tmp_signal_hash_buf, 0x00, single_hash_len);
        if(SEC_OK != (ret = sec_hash((uchar*)double_hash_buf, single_hash_len*2, 
            (uchar*)tmp_signal_hash_buf, single_hash_len )))
        {
            SMSG(true,"[%s] generate hash fail\n\n",MOD);
            ret = ERR_SIGN_FORMAT_GENERATE_HASH_FAIL;
            goto _gen_hash_fail;
        }   

        /*                          */
        mcpy(double_hash_buf, tmp_signal_hash_buf, single_hash_len);

        SMSG(sec_info.bMsg,"[%s] dump file hash (compose):\n",MOD);
        dump_buf((uchar*)double_hash_buf, single_hash_len);
    }

    SMSG(true,"-----------------------\n");       
    SMSG(true,"[%s] read files done\n",MOD);
    SMSG(true,"-----------------------\n\n"); 
    
    SMSG(sec_info.bMsg,"[%s] file list data(final):\n",MOD);
    dump_buf((uchar*)double_hash_buf, single_hash_len*2);

_gen_hash_fail:
    ASF_FREE(tmp_signal_hash_buf);
_malloc_hash_buf_fail:
_read_file_list_fail:
    ASF_FREE(file_buf);
_malloc_file_buf_fail:
_end:

    return ret;
}
    
/*                                                                         
                             
                                                                          */
int sec_signfmt_calculate_filelist_hash(char* part_name, SEC_IMG_U *img_if, char *file_path, 
    char *hash_buf, unsigned int hash_len, char *ext_buf)
{
    unsigned int ret = SEC_OK;
    ASF_FILE fp = ASF_FILE_NULL;
    SEC_IMG_HEADER img_hdr;
    uint32 single_hash_len = 0;
    uchar *verify_buf = NULL;
    uint32 file_content_len = 0;
    uint32 ext_hdr_len = 0;
    uint32 verify_len = 0;
    uint32 data_offset = 0;
    uint32 data_sec_len = 0;

    memset(&img_hdr, 0x00, SEC_IMG_HEADER_SIZE);

    /*             */
    if(SEC_OK != (ret = sec_signfmt_verify_file(file_path, &img_hdr, &data_offset, &data_sec_len)))
    {
        goto _end;
    }

    /*                       */
    if (NULL == file_path)
    {
        SMSG(true,"[%s] doesn't initialize fs sec file list '%s'\n",MOD,file_path);
        ret = ERR_FS_SEC_LIST_NOT_SPECIFY;
        goto _end;
    }

    /*                          */
    fp = ASF_OPEN(file_path);
    if (ASF_FILE_ERROR(fp))
    {
        SMSG(true,"[%s] open '%s' fail\n",MOD,file_path);
        ret = ERR_FS_OPEN_SEC_FILE_FAIL;
        goto _end;
    }

    if(SECCFG_V3 == get_seccfg_ver())
    {
        /*                                                     */
        file_content_len = 0;
    }
    else
    {
        file_content_len = shdr_sign_len(&img_if->v1.header);
    }

    /*                                                */
    if (is_signfmt_v4(&img_hdr) && (SECCFG_V3 == get_seccfg_ver()))
    {
        /*                                      */
        single_hash_len = sec_signfmt_get_hash_length_v4(&img_if->v3, fp, &img_hdr, ext_buf);

        /*                        */
        file_content_len = single_hash_len*2;
        ext_hdr_len = img_if->v3.ext_len - img_if->v3.header.v4.signature_length;
        verify_len = SEC_IMG_HEADER_SIZE + file_content_len + ext_hdr_len;
    }
    else if (is_signfmt_v3(&img_hdr) && (SECCFG_V3 == get_seccfg_ver()))
    {
        /*                                      */
        single_hash_len = sec_signfmt_get_hash_length_v3(&img_if->v3, fp, &img_hdr, ext_buf);

        /*                        */
        file_content_len = single_hash_len*2;
        ext_hdr_len = img_if->v3.ext_len - img_if->v3.header.v3.signature_length;
        verify_len = SEC_IMG_HEADER_SIZE + file_content_len + ext_hdr_len;
    }
    else
    {
        single_hash_len = sec_signfmt_get_hash_length_v2();

        /*                                             */
        if(file_content_len<single_hash_len*2)
        {
            file_content_len = single_hash_len*2;
        }            
        verify_len = SEC_IMG_HEADER_SIZE + file_content_len;
    }

    /*                                */
    verify_buf = ASF_MALLOC(verify_len);
    if(NULL == verify_buf)
    {   
        ret = ERR_FS_READ_BUF_ALLOCATE_FAIL;
        goto _malloc_fail;
    }
    memset(verify_buf, 0x00, verify_len);


    /*                                                                                              */
    if(SEC_OK != (ret = sec_signfmt_calculate_filelist_hash_core(fp, (char*)verify_buf+SEC_IMG_HEADER_SIZE, file_content_len, single_hash_len)))
    {
        goto _end;
    }

    /*                             */
    if( is_signfmt_v4(&img_hdr) && SECCFG_V3 == get_seccfg_ver())
    {
        mcpy(verify_buf, (char*)&img_if->v3.header.va, SEC_IMG_HEADER_SIZE);
        mcpy(verify_buf+SEC_IMG_HEADER_SIZE+file_content_len, (char*)ext_buf+img_if->v3.ext_offset+img_if->v3.header.v4.signature_length, ext_hdr_len);
    }
    else if( is_signfmt_v3(&img_hdr) && SECCFG_V3 == get_seccfg_ver())
    {
        mcpy(verify_buf, (char*)&img_if->v3.header.va, SEC_IMG_HEADER_SIZE);
        mcpy(verify_buf+SEC_IMG_HEADER_SIZE+file_content_len, (char*)ext_buf+img_if->v3.ext_offset+img_if->v3.header.v3.signature_length, ext_hdr_len);
    }
    else
    {
        mcpy(verify_buf, (char*)&img_if->v1.header.va, SEC_IMG_HEADER_SIZE);
    }


    /*                     */
    if(SEC_OK != (ret = sec_hash((uchar*)verify_buf, verify_len, (uchar*)hash_buf, hash_len )))
    {
        SMSG(true,"[%s] generate hash fail\n\n",MOD);
        ret = ERR_SIGN_FORMAT_GENERATE_HASH_FAIL;
        goto _gen_hash_fail;
    } 

    SMSG(sec_info.bMsg,"[%s] file list hash(final):\n",MOD);

    dump_buf((uchar*)hash_buf, hash_len);

    SMSG(sec_info.bMsg,"[%s] generate file list hash pass\n\n",MOD);
    
_gen_hash_fail:
    ASF_FREE(verify_buf);
_malloc_fail:
    ASF_CLOSE(fp);
_end:

    return ret;
}

/*                                                                         
                              
                                                                          */
int sec_signfmt_calculate_image_hash(char* part_name, SEC_IMG_U *img_if, char *hash_buf, unsigned int hash_len, char *ext_buf)
{
    unsigned int ret = SEC_OK;

    /*                        */
    if (SECCFG_V3 == get_seccfg_ver() && is_signfmt_v4((SEC_IMG_HEADER*)&img_if->v3.header.va))
    {
        if(SEC_OK != (ret = sec_signfmt_calculate_image_hash_v4(part_name, &img_if->v3, hash_buf, hash_len, ext_buf)))
        {
            goto _end;
        }
    }
    else if (SECCFG_V3 == get_seccfg_ver() && is_signfmt_v3((SEC_IMG_HEADER*)&img_if->v3.header.va))
    {
        if(SEC_OK != (ret = sec_signfmt_calculate_image_hash_v3(part_name, &img_if->v3, hash_buf, hash_len, ext_buf)))
        {
            goto _end;
        }
    }
    else
    {
        if(SEC_OK != (ret = sec_signfmt_calculate_image_hash_v2(part_name, (SEC_IMG_HEADER*)&img_if->v1.header.va, (uint32)img_if->v1.image_type, hash_buf, hash_len)))
        {
            goto _end;
        }
    }

_end:

    return ret;
}

