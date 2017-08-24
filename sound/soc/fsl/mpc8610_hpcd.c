/**
 * Freescale MPC8610HPCD ALSA SoC Machine driver
 *
 * Author: Timur Tabi <timur@freescale.com>
 *
 * Copyright 2007-2010 Freescale Semiconductor, Inc.
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

/*                                            */
static phys_addr_t guts_phys;

/* 
                                                       
  
                                                                        
                                                                 
 */
struct mpc8610_hpcd_data {
	struct snd_soc_dai_link dai[2];
	struct snd_soc_card card;
	unsigned int dai_format;
	unsigned int codec_clk_direction;
	unsigned int cpu_clk_direction;
	unsigned int clk_frequency;
	unsigned int ssi_id;		/*                         */
	unsigned int dma_id[2];		/*                         */
	unsigned int dma_channel_id[2]; /*                        */
	char codec_dai_name[DAI_NAME_SIZE];
	char platform_name[2][DAI_NAME_SIZE]; /*                          */
};

/* 
                                                   
  
                                                                   
  
                                                  
 */
static int mpc8610_hpcd_machine_probe(struct snd_soc_card *card)
{
	struct mpc8610_hpcd_data *machine_data =
		container_of(card, struct mpc8610_hpcd_data, card);
	struct ccsr_guts __iomem *guts;

	guts = ioremap(guts_phys, sizeof(struct ccsr_guts));
	if (!guts) {
		dev_err(card->dev, "could not map global utilities\n");
		return -ENOMEM;
	}

	/*                                                        */
	guts_set_dmacr(guts, machine_data->dma_id[0],
		       machine_data->dma_channel_id[0],
		       CCSR_GUTS_DMACR_DEV_SSI);
	guts_set_dmacr(guts, machine_data->dma_id[1],
		       machine_data->dma_channel_id[1],
		       CCSR_GUTS_DMACR_DEV_SSI);

	guts_set_pmuxcr_dma(guts, machine_data->dma_id[0],
			    machine_data->dma_channel_id[0], 0);
	guts_set_pmuxcr_dma(guts, machine_data->dma_id[1],
			    machine_data->dma_channel_id[1], 0);

	switch (machine_data->ssi_id) {
	case 0:
		clrsetbits_be32(&guts->pmuxcr,
			CCSR_GUTS_PMUXCR_SSI1_MASK, CCSR_GUTS_PMUXCR_SSI1_SSI);
		break;
	case 1:
		clrsetbits_be32(&guts->pmuxcr,
			CCSR_GUTS_PMUXCR_SSI2_MASK, CCSR_GUTS_PMUXCR_SSI2_SSI);
		break;
	}

	iounmap(guts);

	return 0;
}

/* 
                                                                           
  
                                                                         
                                                                        
                     
 */
static int mpc8610_hpcd_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mpc8610_hpcd_data *machine_data =
		container_of(rtd->card, struct mpc8610_hpcd_data, card);
	struct device *dev = rtd->card->dev;
	int ret = 0;

	/*                                                    */
	ret = snd_soc_dai_set_fmt(rtd->codec_dai, machine_data->dai_format);
	if (ret < 0) {
		dev_err(dev, "could not set codec driver audio format\n");
		return ret;
	}

	/*
                                                                      
                      
  */
	ret = snd_soc_dai_set_sysclk(rtd->codec_dai, 0,
				     machine_data->clk_frequency,
				     machine_data->codec_clk_direction);
	if (ret < 0) {
		dev_err(dev, "could not set codec driver clock params\n");
		return ret;
	}

	return 0;
}

/* 
                                                       
  
                                                                      
                                            
 */
static int mpc8610_hpcd_machine_remove(struct snd_soc_card *card)
{
	struct mpc8610_hpcd_data *machine_data =
		container_of(card, struct mpc8610_hpcd_data, card);
	struct ccsr_guts __iomem *guts;

	guts = ioremap(guts_phys, sizeof(struct ccsr_guts));
	if (!guts) {
		dev_err(card->dev, "could not map global utilities\n");
		return -ENOMEM;
	}

	/*                            */

	guts_set_dmacr(guts, machine_data->dma_id[0],
		       machine_data->dma_channel_id[0], 0);
	guts_set_dmacr(guts, machine_data->dma_id[1],
		       machine_data->dma_channel_id[1], 0);

	switch (machine_data->ssi_id) {
	case 0:
		clrsetbits_be32(&guts->pmuxcr,
			CCSR_GUTS_PMUXCR_SSI1_MASK, CCSR_GUTS_PMUXCR_SSI1_LA);
		break;
	case 1:
		clrsetbits_be32(&guts->pmuxcr,
			CCSR_GUTS_PMUXCR_SSI2_MASK, CCSR_GUTS_PMUXCR_SSI2_LA);
		break;
	}

	iounmap(guts);

	return 0;
}

/* 
                                                   
 */
static struct snd_soc_ops mpc8610_hpcd_ops = {
	.startup = mpc8610_hpcd_startup,
};

/* 
                                                                     
  
                                                                            
                                                                          
                                                          
 */
static int mpc8610_hpcd_probe(struct platform_device *pdev)
{
	struct device *dev = pdev->dev.parent;
	/*                                                                 */
	struct platform_device *ssi_pdev =
		container_of(dev, struct platform_device, dev);
	struct device_node *np = ssi_pdev->dev.of_node;
	struct device_node *codec_np = NULL;
	struct mpc8610_hpcd_data *machine_data;
	int ret = -ENODEV;
	const char *sprop;
	const u32 *iprop;

	/*                                   */
	codec_np = of_parse_phandle(np, "codec-handle", 0);
	if (!codec_np) {
		dev_err(dev, "invalid codec node\n");
		return -EINVAL;
	}

	machine_data = kzalloc(sizeof(struct mpc8610_hpcd_data), GFP_KERNEL);
	if (!machine_data) {
		ret = -ENOMEM;
		goto error_alloc;
	}

	machine_data->dai[0].cpu_dai_name = dev_name(&ssi_pdev->dev);
	machine_data->dai[0].ops = &mpc8610_hpcd_ops;

	/*                                            */
	machine_data->dai[0].codec_of_node = codec_np;

	/*                                                       */
	machine_data->dai[0].codec_dai_name = "cs4270-hifi";

	/*                                                                 
                                                                     
                              
  */
	memcpy(&machine_data->dai[1], &machine_data->dai[0],
	       sizeof(struct snd_soc_dai_link));

	/*                   */
	iprop = of_get_property(np, "cell-index", NULL);
	if (!iprop) {
		dev_err(&pdev->dev, "cell-index property not found\n");
		ret = -EINVAL;
		goto error;
	}
	machine_data->ssi_id = be32_to_cpup(iprop);

	/*                                            */
	sprop = of_get_property(np, "fsl,mode", NULL);
	if (!sprop) {
		dev_err(&pdev->dev, "fsl,mode property not found\n");
		ret = -EINVAL;
		goto error;
	}

	if (strcasecmp(sprop, "i2s-slave") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBM_CFM;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_OUT;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_IN;

		/*                                                             
                                                                  
                      
   */
		iprop = of_get_property(codec_np, "clock-frequency", NULL);
		if (!iprop || !*iprop) {
			dev_err(&pdev->dev, "codec bus-frequency "
				"property is missing or invalid\n");
			ret = -EINVAL;
			goto error;
		}
		machine_data->clk_frequency = be32_to_cpup(iprop);
	} else if (strcasecmp(sprop, "i2s-master") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBS_CFS;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_IN;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else if (strcasecmp(sprop, "lj-slave") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_LEFT_J | SND_SOC_DAIFMT_CBM_CFM;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_OUT;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_IN;
	} else if (strcasecmp(sprop, "lj-master") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_LEFT_J | SND_SOC_DAIFMT_CBS_CFS;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_IN;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else if (strcasecmp(sprop, "rj-slave") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_RIGHT_J | SND_SOC_DAIFMT_CBM_CFM;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_OUT;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_IN;
	} else if (strcasecmp(sprop, "rj-master") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_RIGHT_J | SND_SOC_DAIFMT_CBS_CFS;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_IN;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else if (strcasecmp(sprop, "ac97-slave") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_AC97 | SND_SOC_DAIFMT_CBM_CFM;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_OUT;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_IN;
	} else if (strcasecmp(sprop, "ac97-master") == 0) {
		machine_data->dai_format =
			SND_SOC_DAIFMT_AC97 | SND_SOC_DAIFMT_CBS_CFS;
		machine_data->codec_clk_direction = SND_SOC_CLOCK_IN;
		machine_data->cpu_clk_direction = SND_SOC_CLOCK_OUT;
	} else {
		dev_err(&pdev->dev,
			"unrecognized fsl,mode property '%s'\n", sprop);
		ret = -EINVAL;
		goto error;
	}

	if (!machine_data->clk_frequency) {
		dev_err(&pdev->dev, "unknown clock frequency\n");
		ret = -EINVAL;
		goto error;
	}

	/*                                       */
	machine_data->dai[0].platform_name = machine_data->platform_name[0];
	ret = fsl_asoc_get_dma_channel(np, "fsl,playback-dma",
				       &machine_data->dai[0],
				       &machine_data->dma_channel_id[0],
				       &machine_data->dma_id[0]);
	if (ret) {
		dev_err(&pdev->dev, "missing/invalid playback DMA phandle\n");
		goto error;
	}

	/*                                      */
	machine_data->dai[1].platform_name = machine_data->platform_name[1];
	ret = fsl_asoc_get_dma_channel(np, "fsl,capture-dma",
				       &machine_data->dai[1],
				       &machine_data->dma_channel_id[1],
				       &machine_data->dma_id[1]);
	if (ret) {
		dev_err(&pdev->dev, "missing/invalid capture DMA phandle\n");
		goto error;
	}

	/*                                     */
	machine_data->dai[0].stream_name = "playback";
	machine_data->dai[1].stream_name = "capture";
	machine_data->dai[0].name = machine_data->dai[0].stream_name;
	machine_data->dai[1].name = machine_data->dai[1].stream_name;

	machine_data->card.probe = mpc8610_hpcd_machine_probe;
	machine_data->card.remove = mpc8610_hpcd_machine_remove;
	machine_data->card.name = pdev->name; /*                          */
	machine_data->card.owner = THIS_MODULE;
	machine_data->card.dev = &pdev->dev;
	machine_data->card.num_links = 2;
	machine_data->card.dai_link = machine_data->dai;

	/*                    */
	ret = snd_soc_register_card(&machine_data->card);
	if (ret) {
		dev_err(&pdev->dev, "could not register card\n");
		goto error;
	}

	of_node_put(codec_np);

	return 0;

error:
	kfree(machine_data);
error_alloc:
	of_node_put(codec_np);
	return ret;
}

/* 
                                                  
  
                                                               
 */
static int mpc8610_hpcd_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);
	struct mpc8610_hpcd_data *machine_data =
		container_of(card, struct mpc8610_hpcd_data, card);

	snd_soc_unregister_card(card);
	kfree(machine_data);

	return 0;
}

static struct platform_driver mpc8610_hpcd_driver = {
	.probe = mpc8610_hpcd_probe,
	.remove = mpc8610_hpcd_remove,
	.driver = {
		/*                                                              
                          
   */
		.name = "snd-soc-mpc8610hpcd",
		.owner = THIS_MODULE,
	},
};

/* 
                                                    
  
                                                      
 */
static int __init mpc8610_hpcd_init(void)
{
	struct device_node *guts_np;
	struct resource res;

	pr_info("Freescale MPC8610 HPCD ALSA SoC machine driver\n");

	/*                                                            */
	guts_np = of_find_compatible_node(NULL, NULL, "fsl,mpc8610-guts");
	if (of_address_to_resource(guts_np, 0, &res)) {
		pr_err("mpc8610-hpcd: missing/invalid global utilities node\n");
		return -EINVAL;
	}
	guts_phys = res.start;

	return platform_driver_register(&mpc8610_hpcd_driver);
}

/* 
                                         
  
                                                        
 */
static void __exit mpc8610_hpcd_exit(void)
{
	platform_driver_unregister(&mpc8610_hpcd_driver);
}

module_init(mpc8610_hpcd_init);
module_exit(mpc8610_hpcd_exit);

MODULE_AUTHOR("Timur Tabi <timur@freescale.com>");
MODULE_DESCRIPTION("Freescale MPC8610 HPCD ALSA SoC machine driver");
MODULE_LICENSE("GPL v2");
