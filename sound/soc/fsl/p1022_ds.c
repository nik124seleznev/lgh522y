/**
 * Freescale P1022DS ALSA SoC Machine driver
 *
 * Author: Timur Tabi <timur@freescale.com>
 *
 * Copyright 2010 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <sound/soc.h>
#include <asm/fsl_guts.h>

#include "fsl_dma.h"
#include "fsl_ssi.h"
#include "fsl_utils.h"

/*                                                  */

#define CCSR_GUTS_PMUXCR_UART0_I2C1_MASK	0x0001c000
#define CCSR_GUTS_PMUXCR_UART0_I2C1_UART0_SSI	0x00010000
#define CCSR_GUTS_PMUXCR_UART0_I2C1_SSI		0x00018000

#define CCSR_GUTS_PMUXCR_SSI_DMA_TDM_MASK	0x00000c00
#define CCSR_GUTS_PMUXCR_SSI_DMA_TDM_SSI	0x00000000

#define CCSR_GUTS_DMUXCR_PAD	1	/*                                   */
#define CCSR_GUTS_DMUXCR_SSI	2	/*                                   */

/*
                                     
  
                                                                           
                                                                          
                                                                       
        
  
                                  
                                  
                                                        
                                                                 
 */
static inline void guts_set_dmuxcr(struct ccsr_guts __iomem *guts,
	unsigned int co, unsigned int ch, unsigned int device)
{
	unsigned int shift = 16 + (8 * (1 - co) + 2 * (3 - ch));

	clrsetbits_be32(&guts->dmuxcr, 3 << shift, device << shift);
}

/*                                            */
static phys_addr_t guts_phys;

/* 
                                                  
  
                                                                        
                                                             
 */
struct machine_data {
	struct snd_soc_dai_link dai[2];
	struct snd_soc_card card;
	unsigned int dai_format;
	unsigned int codec_clk_direction;
	unsigned int cpu_clk_direction;
	unsigned int clk_frequency;
	unsigned int ssi_id;		/*                         */
	unsigned int dma_id[2];		/*                         */
	unsigned int dma_channel_id[2]; /*                        */
	char platform_name[2][DAI_NAME_SIZE]; /*                          */
};

/* 
                                               
  
                                                                   
  
                                                  
 */
static int p1022_ds_machine_probe(struct snd_soc_card *card)
{
	struct machine_data *mdata =
		container_of(card, struct machine_data, card);
	struct ccsr_guts __iomem *guts;

	guts = ioremap(guts_phys, sizeof(struct ccsr_guts));
	if (!guts) {
		dev_err(card->dev, "could not map global utilities\n");
		return -ENOMEM;
	}

	/*                      */
	clrsetbits_be32(&guts->pmuxcr, CCSR_GUTS_PMUXCR_UART0_I2C1_MASK,
			CCSR_GUTS_PMUXCR_UART0_I2C1_UART0_SSI);

	/*                      */
	clrsetbits_be32(&guts->pmuxcr, CCSR_GUTS_PMUXCR_SSI_DMA_TDM_MASK,
			CCSR_GUTS_PMUXCR_SSI_DMA_TDM_SSI);

	/*                            */
	guts_set_dmuxcr(guts, mdata->dma_id[0], mdata->dma_channel_id[0],
			CCSR_GUTS_DMUXCR_SSI);

	guts_set_dmuxcr(guts, mdata->dma_id[1], mdata->dma_channel_id[1],
			CCSR_GUTS_DMUXCR_SSI);

	iounmap(guts);

	return 0;
}

/* 
                                                                       
  
                                                                         
                                                                        
                     
 */
static int p1022_ds_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct machine_data *mdata =
		container_of(rtd->card, struct machine_data, card);
	struct device *dev = rtd->card->dev;
	int ret = 0;

	/*                                                    */
	ret = snd_soc_dai_set_fmt(rtd->codec_dai, mdata->dai_format);
	if (ret < 0) {
		dev_err(dev, "could not set codec driver audio format\n");
		return ret;
	}

	/*
                                                                      
                      
  */
	ret = snd_soc_dai_set_sysclk(rtd->codec_dai, 0, mdata->clk_frequency,
				     mdata->codec_clk_direction);
	if (ret < 0) {
		dev_err(dev, "could not set codec driver clock params\n");
		return ret;
	}

	return 0;
}

/* 
                                                   
  
                                                                      
                                            
 */
static int p1022_ds_machine_remove(struct snd_soc_card *card)
{
	struct machine_data *mdata =
		container_of(card, struct machine_data, card);
	struct ccsr_guts __iomem *guts;

	guts = ioremap(guts_phys, sizeof(struct ccsr_guts));
	if (!guts) {
		dev_err(card->dev, "could not map global utilities\n");
		return -ENOMEM;
	}

	/*                            */
	clrbits32(&guts->pmuxcr, CCSR_GUTS_PMUXCR_UART0_I2C1_MASK);
	clrbits32(&guts->pmuxcr, CCSR_GUTS_PMUXCR_SSI_DMA_TDM_MASK);
	guts_set_dmuxcr(guts, mdata->dma_id[0], mdata->dma_channel_id[0], 0);
	guts_set_dmuxcr(guts, mdata->dma_id[1], mdata->dma_channel_id[1], 0);

	iounmap(guts);

	return 0;
}

/* 
                                               
 */
static struct snd_soc_ops p1022_ds_ops = {
	.startup = p1022_ds_startup,
};

/* 
                                                                 
  
                                                                            
                                                                          
                                                          
 */
static int p1022_ds_probe(struct platform_device *pdev)
{
	struct device *dev = pdev->dev.parent;
	/*                                                                 */
	struct platform_device *ssi_pdev =
		container_of(dev, struct platform_device, dev);
	struct device_node *np = ssi_pdev->dev.of_node;
	struct device_node *codec_np = NULL;
	struct machine_data *mdata;
	int ret = -ENODEV;
	const char *sprop;
	const u32 *iprop;

	/*                                   */
	codec_np = of_parse_phandle(np, "codec-handle", 0);
	if (!codec_np) {
		dev_err(dev, "could not find codec node\n");
		return -EINVAL;
	}

	mdata = kzalloc(sizeof(struct machine_data), GFP_KERNEL);
	if (!mdata) {
		ret = -ENOMEM;
		goto error_put;
	}

	mdata->dai[0].cpu_dai_name = dev_name(&ssi_pdev->dev);
	mdata->dai[0].ops = &p1022_ds_ops;

	/*                                            */
	mdata->dai[0].codec_of_node = codec_np;

	/*                                                                 
                                                                         
                
  */
	memcpy(&mdata->dai[1], &mdata->dai[0], sizeof(struct snd_soc_dai_link));

	/*                                                        */
	mdata->dai[0].codec_dai_name = "wm8776-hifi-playback";
	mdata->dai[1].codec_dai_name = "wm8776-hifi-capture";

	/*                   */
	iprop = of_get_property(np, "cell-index", NULL);
	if (!iprop) {
		dev_err(&pdev->dev, "cell-index property not found\n");
		ret = -EINVAL;
		goto error;
	}
	mdata->ssi_id = be32_to_cpup(iprop);

	/*                                            */
	sprop = of_get_property(np, "fsl,mode", NULL);
	if (!sprop) {
		dev_err(&pdev->dev, "fsl,mode property not found\n");
		ret = -EINVAL;
		goto error;
	}

	if (strcasecmp(sprop, "i2s-slave") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBM_CFM;
		mdata->codec_clk_direction = SND_SOC_CLOCK_OUT;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_IN;

		/*                                                             
                                                                  
                      
   */
		iprop = of_get_property(codec_np, "clock-frequency", NULL);
		if (!iprop || !*iprop) {
			dev_err(&pdev->dev, "codec bus-frequency "
				"property is missing or invalid\n");
			ret = -EINVAL;
			goto error;
		}
		mdata->clk_frequency = be32_to_cpup(iprop);
	} else if (strcasecmp(sprop, "i2s-master") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBS_CFS;
		mdata->codec_clk_direction = SND_SOC_CLOCK_IN;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else if (strcasecmp(sprop, "lj-slave") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_LEFT_J | SND_SOC_DAIFMT_CBM_CFM;
		mdata->codec_clk_direction = SND_SOC_CLOCK_OUT;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_IN;
	} else if (strcasecmp(sprop, "lj-master") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_LEFT_J | SND_SOC_DAIFMT_CBS_CFS;
		mdata->codec_clk_direction = SND_SOC_CLOCK_IN;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else if (strcasecmp(sprop, "rj-slave") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_RIGHT_J | SND_SOC_DAIFMT_CBM_CFM;
		mdata->codec_clk_direction = SND_SOC_CLOCK_OUT;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_IN;
	} else if (strcasecmp(sprop, "rj-master") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_RIGHT_J | SND_SOC_DAIFMT_CBS_CFS;
		mdata->codec_clk_direction = SND_SOC_CLOCK_IN;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else if (strcasecmp(sprop, "ac97-slave") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_AC97 | SND_SOC_DAIFMT_CBM_CFM;
		mdata->codec_clk_direction = SND_SOC_CLOCK_OUT;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_IN;
	} else if (strcasecmp(sprop, "ac97-master") == 0) {
		mdata->dai_format = SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_AC97 | SND_SOC_DAIFMT_CBS_CFS;
		mdata->codec_clk_direction = SND_SOC_CLOCK_IN;
		mdata->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else {
		dev_err(&pdev->dev,
			"unrecognized fsl,mode property '%s'\n", sprop);
		ret = -EINVAL;
		goto error;
	}

	if (!mdata->clk_frequency) {
		dev_err(&pdev->dev, "unknown clock frequency\n");
		ret = -EINVAL;
		goto error;
	}

	/*                                       */
	mdata->dai[0].platform_name = mdata->platform_name[0];
	ret = fsl_asoc_get_dma_channel(np, "fsl,playback-dma", &mdata->dai[0],
				       &mdata->dma_channel_id[0],
				       &mdata->dma_id[0]);
	if (ret) {
		dev_err(&pdev->dev, "missing/invalid playback DMA phandle\n");
		goto error;
	}

	/*                                      */
	mdata->dai[1].platform_name = mdata->platform_name[1];
	ret = fsl_asoc_get_dma_channel(np, "fsl,capture-dma", &mdata->dai[1],
				       &mdata->dma_channel_id[1],
				       &mdata->dma_id[1]);
	if (ret) {
		dev_err(&pdev->dev, "missing/invalid capture DMA phandle\n");
		goto error;
	}

	/*                                     */
	mdata->dai[0].stream_name = "playback";
	mdata->dai[1].stream_name = "capture";
	mdata->dai[0].name = mdata->dai[0].stream_name;
	mdata->dai[1].name = mdata->dai[1].stream_name;

	mdata->card.probe = p1022_ds_machine_probe;
	mdata->card.remove = p1022_ds_machine_remove;
	mdata->card.name = pdev->name; /*                          */
	mdata->card.owner = THIS_MODULE;
	mdata->card.dev = &pdev->dev;
	mdata->card.num_links = 2;
	mdata->card.dai_link = mdata->dai;

	/*                    */
	ret = snd_soc_register_card(&mdata->card);
	if (ret) {
		dev_err(&pdev->dev, "could not register card\n");
		goto error;
	}

	of_node_put(codec_np);

	return 0;

error:
	kfree(mdata);
error_put:
	of_node_put(codec_np);
	return ret;
}

/* 
                                              
  
                                                               
 */
static int p1022_ds_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);
	struct machine_data *mdata =
		container_of(card, struct machine_data, card);

	snd_soc_unregister_card(card);
	kfree(mdata);

	return 0;
}

static struct platform_driver p1022_ds_driver = {
	.probe = p1022_ds_probe,
	.remove = p1022_ds_remove,
	.driver = {
		/*
                                                                  
                          
   */
		.name = "snd-soc-p1022ds",
		.owner = THIS_MODULE,
	},
};

/* 
                                                
  
                                                      
 */
static int __init p1022_ds_init(void)
{
	struct device_node *guts_np;
	struct resource res;

	/*                                                            */
	guts_np = of_find_compatible_node(NULL, NULL, "fsl,p1022-guts");
	if (of_address_to_resource(guts_np, 0, &res)) {
		pr_err("snd-soc-p1022ds: missing/invalid global utils node\n");
		of_node_put(guts_np);
		return -EINVAL;
	}
	guts_phys = res.start;
	of_node_put(guts_np);

	return platform_driver_register(&p1022_ds_driver);
}

/* 
                                     
  
                                                        
 */
static void __exit p1022_ds_exit(void)
{
	platform_driver_unregister(&p1022_ds_driver);
}

module_init(p1022_ds_init);
module_exit(p1022_ds_exit);

MODULE_AUTHOR("Timur Tabi <timur@freescale.com>");
MODULE_DESCRIPTION("Freescale P1022 DS ALSA SoC machine driver");
MODULE_LICENSE("GPL v2");
