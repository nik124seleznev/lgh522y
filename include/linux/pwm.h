#ifndef __LINUX_PWM_H
#define __LINUX_PWM_H

#include <linux/err.h>
#include <linux/of.h>

struct pwm_device;
struct seq_file;

#if IS_ENABLED(CONFIG_PWM) || IS_ENABLED(CONFIG_HAVE_PWM)
/*
                                     
 */
struct pwm_device *pwm_request(int pwm_id, const char *label);

/*
                               
 */
void pwm_free(struct pwm_device *pwm);

/*
                                                 
 */
int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns);

/*
                                           
 */
int pwm_enable(struct pwm_device *pwm);

/*
                                           
 */
void pwm_disable(struct pwm_device *pwm);
#else
static inline struct pwm_device *pwm_request(int pwm_id, const char *label)
{
	return ERR_PTR(-ENODEV);
}

static inline void pwm_free(struct pwm_device *pwm)
{
}

static inline int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns)
{
	return -EINVAL;
}

static inline int pwm_enable(struct pwm_device *pwm)
{
	return -EINVAL;
}

static inline void pwm_disable(struct pwm_device *pwm)
{
}
#endif

struct pwm_chip;

/* 
                                               
                                                                    
                                                                 
         
                                                                     
                                                                  
         
 */
enum pwm_polarity {
	PWM_POLARITY_NORMAL,
	PWM_POLARITY_INVERSED,
};

enum {
	PWMF_REQUESTED = 1 << 0,
	PWMF_ENABLED = 1 << 1,
};

struct pwm_device {
	const char		*label;
	unsigned long		flags;
	unsigned int		hwpwm;
	unsigned int		pwm;
	struct pwm_chip		*chip;
	void			*chip_data;

	unsigned int		period; /*                */
};

static inline void pwm_set_period(struct pwm_device *pwm, unsigned int period)
{
	if (pwm)
		pwm->period = period;
}

static inline unsigned int pwm_get_period(struct pwm_device *pwm)
{
	return pwm ? pwm->period : 0;
}

/*
                                                            
 */
int pwm_set_polarity(struct pwm_device *pwm, enum pwm_polarity polarity);

/* 
                                             
                                               
                                         
                                                                
                                                    
                                      
                                        
                                                          
                                                                 
 */
struct pwm_ops {
	int			(*request)(struct pwm_chip *chip,
					   struct pwm_device *pwm);
	void			(*free)(struct pwm_chip *chip,
					struct pwm_device *pwm);
	int			(*config)(struct pwm_chip *chip,
					  struct pwm_device *pwm,
					  int duty_ns, int period_ns);
	int			(*set_polarity)(struct pwm_chip *chip,
					  struct pwm_device *pwm,
					  enum pwm_polarity polarity);
	int			(*enable)(struct pwm_chip *chip,
					  struct pwm_device *pwm);
	void			(*disable)(struct pwm_chip *chip,
					   struct pwm_device *pwm);
#ifdef CONFIG_DEBUG_FS
	void			(*dbg_show)(struct pwm_chip *chip,
					    struct seq_file *s);
#endif
	struct module		*owner;
};

/* 
                                              
                                  
                                    
                                          
                                                     
                                                
                                                         
                                                                     
                                   
 */
struct pwm_chip {
	struct device		*dev;
	struct list_head	list;
	const struct pwm_ops	*ops;
	int			base;
	unsigned int		npwm;

	struct pwm_device	*pwms;

	struct pwm_device *	(*of_xlate)(struct pwm_chip *pc,
					    const struct of_phandle_args *args);
	unsigned int		of_pwm_n_cells;
	bool			can_sleep;
};

#if IS_ENABLED(CONFIG_PWM)
int pwm_set_chip_data(struct pwm_device *pwm, void *data);
void *pwm_get_chip_data(struct pwm_device *pwm);

int pwmchip_add(struct pwm_chip *chip);
int pwmchip_remove(struct pwm_chip *chip);
struct pwm_device *pwm_request_from_chip(struct pwm_chip *chip,
					 unsigned int index,
					 const char *label);

struct pwm_device *of_pwm_xlate_with_flags(struct pwm_chip *pc,
		const struct of_phandle_args *args);

struct pwm_device *pwm_get(struct device *dev, const char *con_id);
struct pwm_device *of_pwm_get(struct device_node *np, const char *con_id);
void pwm_put(struct pwm_device *pwm);

struct pwm_device *devm_pwm_get(struct device *dev, const char *con_id);
struct pwm_device *devm_of_pwm_get(struct device *dev, struct device_node *np,
				   const char *con_id);
void devm_pwm_put(struct device *dev, struct pwm_device *pwm);

bool pwm_can_sleep(struct pwm_device *pwm);
#else
static inline int pwm_set_chip_data(struct pwm_device *pwm, void *data)
{
	return -EINVAL;
}

static inline void *pwm_get_chip_data(struct pwm_device *pwm)
{
	return NULL;
}

static inline int pwmchip_add(struct pwm_chip *chip)
{
	return -EINVAL;
}

static inline int pwmchip_remove(struct pwm_chip *chip)
{
	return -EINVAL;
}

static inline struct pwm_device *pwm_request_from_chip(struct pwm_chip *chip,
						       unsigned int index,
						       const char *label)
{
	return ERR_PTR(-ENODEV);
}

static inline struct pwm_device *pwm_get(struct device *dev,
					 const char *consumer)
{
	return ERR_PTR(-ENODEV);
}

static inline struct pwm_device *of_pwm_get(struct device_node *np,
					    const char *con_id)
{
	return ERR_PTR(-ENODEV);
}

static inline void pwm_put(struct pwm_device *pwm)
{
}

static inline struct pwm_device *devm_pwm_get(struct device *dev,
					      const char *consumer)
{
	return ERR_PTR(-ENODEV);
}

static inline struct pwm_device *devm_of_pwm_get(struct device *dev,
						 struct device_node *np,
						 const char *con_id)
{
	return ERR_PTR(-ENODEV);
}

static inline void devm_pwm_put(struct device *dev, struct pwm_device *pwm)
{
}

static inline bool pwm_can_sleep(struct pwm_device *pwm)
{
	return false;
}
#endif

struct pwm_lookup {
	struct list_head list;
	const char *provider;
	unsigned int index;
	const char *dev_id;
	const char *con_id;
};

#define PWM_LOOKUP(_provider, _index, _dev_id, _con_id)	\
	{						\
		.provider = _provider,			\
		.index = _index,			\
		.dev_id = _dev_id,			\
		.con_id = _con_id,			\
	}

#if IS_ENABLED(CONFIG_PWM)
void pwm_add_table(struct pwm_lookup *table, size_t num);
#else
static inline void pwm_add_table(struct pwm_lookup *table, size_t num)
{
}
#endif

#endif /*               */
