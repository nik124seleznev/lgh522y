/*
 * Copyright (C) 2009 Francisco Jerez.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <linux/module.h>

#include <drm/drm_encoder_slave.h>

/* 
                                                         
                    
                                                                    
                                                      
                                                           
               
                                                                 
                                        
  
                                                                       
                                                                 
                                                                      
                                          
  
                                                                     
                
  
                                                                      
                                                        
 */
int drm_i2c_encoder_init(struct drm_device *dev,
			 struct drm_encoder_slave *encoder,
			 struct i2c_adapter *adap,
			 const struct i2c_board_info *info)
{
	struct module *module = NULL;
	struct i2c_client *client;
	struct drm_i2c_encoder_driver *encoder_drv;
	int err = 0;

	request_module("%s%s", I2C_MODULE_PREFIX, info->type);

	client = i2c_new_device(adap, info);
	if (!client) {
		err = -ENOMEM;
		goto fail;
	}

	if (!client->driver) {
		err = -ENODEV;
		goto fail_unregister;
	}

	module = client->driver->driver.owner;
	if (!try_module_get(module)) {
		err = -ENODEV;
		goto fail_unregister;
	}

	encoder->bus_priv = client;

	encoder_drv = to_drm_i2c_encoder_driver(client->driver);

	err = encoder_drv->encoder_init(client, dev, encoder);
	if (err)
		goto fail_unregister;

	if (info->platform_data)
		encoder->slave_funcs->set_config(&encoder->base,
						 info->platform_data);

	return 0;

fail_unregister:
	i2c_unregister_device(client);
	module_put(module);
fail:
	return err;
}
EXPORT_SYMBOL(drm_i2c_encoder_init);

/* 
                                                                         
                                            
  
                                                                 
                                                      
 */
void drm_i2c_encoder_destroy(struct drm_encoder *drm_encoder)
{
	struct drm_encoder_slave *encoder = to_encoder_slave(drm_encoder);
	struct i2c_client *client = drm_i2c_encoder_get_client(drm_encoder);
	struct module *module = client->driver->driver.owner;

	i2c_unregister_device(client);
	encoder->bus_priv = NULL;

	module_put(module);
}
EXPORT_SYMBOL(drm_i2c_encoder_destroy);

/*
                                                                    
 */

static inline struct drm_encoder_slave_funcs *
get_slave_funcs(struct drm_encoder *enc)
{
	return to_encoder_slave(enc)->slave_funcs;
}

void drm_i2c_encoder_dpms(struct drm_encoder *encoder, int mode)
{
	get_slave_funcs(encoder)->dpms(encoder, mode);
}
EXPORT_SYMBOL(drm_i2c_encoder_dpms);

bool drm_i2c_encoder_mode_fixup(struct drm_encoder *encoder,
		const struct drm_display_mode *mode,
		struct drm_display_mode *adjusted_mode)
{
	return get_slave_funcs(encoder)->mode_fixup(encoder, mode, adjusted_mode);
}
EXPORT_SYMBOL(drm_i2c_encoder_mode_fixup);

void drm_i2c_encoder_prepare(struct drm_encoder *encoder)
{
	drm_i2c_encoder_dpms(encoder, DRM_MODE_DPMS_OFF);
}
EXPORT_SYMBOL(drm_i2c_encoder_prepare);

void drm_i2c_encoder_commit(struct drm_encoder *encoder)
{
	drm_i2c_encoder_dpms(encoder, DRM_MODE_DPMS_ON);
}
EXPORT_SYMBOL(drm_i2c_encoder_commit);

void drm_i2c_encoder_mode_set(struct drm_encoder *encoder,
		struct drm_display_mode *mode,
		struct drm_display_mode *adjusted_mode)
{
	get_slave_funcs(encoder)->mode_set(encoder, mode, adjusted_mode);
}
EXPORT_SYMBOL(drm_i2c_encoder_mode_set);

enum drm_connector_status drm_i2c_encoder_detect(struct drm_encoder *encoder,
	    struct drm_connector *connector)
{
	return get_slave_funcs(encoder)->detect(encoder, connector);
}
EXPORT_SYMBOL(drm_i2c_encoder_detect);

void drm_i2c_encoder_save(struct drm_encoder *encoder)
{
	get_slave_funcs(encoder)->save(encoder);
}
EXPORT_SYMBOL(drm_i2c_encoder_save);

void drm_i2c_encoder_restore(struct drm_encoder *encoder)
{
	get_slave_funcs(encoder)->restore(encoder);
}
EXPORT_SYMBOL(drm_i2c_encoder_restore);
