/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSOC Control C1 MCU: VADC SCAN Example
*              for ModusToolbox. This example shows how to configure ADC for 
*              Continuous Scan mode.ADC result is used to change status of LED on kit.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cybsp.h"
#include "cy_utils.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
*******************************************************************************/

/* Define macros for KIT_PSC1M3_EVK kit */
#define RES_REG_NUMBER                      (10)
#define CHANNEL_NUMBER                      (7U)
#define ADC_CONVERSION_EVENT_HANDLER        VADC_SR0_INTERRUPT_HANDLER
#define INTERRUPT_PRIORITY_NODE_ID          VADC_SR0_IRQN
#define ADC_MEASUREMENT_ICLASS_NUM          (0U)

/* Define macro to enable/disable printing of debug messages */
#define ENABLE_DEBUG_PRINT                  (0)

#if ENABLE_DEBUG_PRINT
static bool LED_TOGGLE = false;
#endif

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* Stores the latest ADC conversion result read from the result register */
static volatile unsigned long g_result_adc_measurement;

/*******************************************************************************
* Function Name: ADC_CONVERSION_EVENT_HANDLER
********************************************************************************
* Summary:
* This is the interrupt handler function for the ADC after conversion.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ADC_CONVERSION_EVENT_HANDLER(void)
{
    /*Read out conversion results*/
    g_result_adc_measurement=Cy_VADC_GROUP_GetResult(VADC_GROUP_HW, RES_REG_NUMBER);
    
    #if !(ENABLE_DEBUG_PRINT)
    /* Prints the result in UART Terminal */
    printf("ADC Result value is %lx \r\n", g_result_adc_measurement);
    #endif
    
    if(g_result_adc_measurement >= 2000)
    {
        /*Compare the result counts  */
        Cy_GPIO_SetOutputLow(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
    }
    else
    {
        #if ENABLE_DEBUG_PRINT
        if(!LED_TOGGLE)
        {
        printf("LED Toggled\r\n");
        LED_TOGGLE = true;
        }
        #endif

        Cy_GPIO_SetOutputHigh(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
    }
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function.
* It sets up a VADC for continuous scan using background scan source.
* ADC result is available after conversion is completed inside event handler function.
* ADC result register value is compared inside event handler function 
* On board LED glows high when ADC counts are more than 2000.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /*Initialize the device and board peripherals*/
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize printf retarget */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_HW);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    #if ENABLE_DEBUG_PRINT
    printf("Initialization done\r\n");
    #endif

    /*Add all channels into the Background Request Source Channel Select Register*/
    Cy_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, VADC_GROUP_NUM, CHANNEL_NUMBER);

    /*Enable Background Continuous Scan Request source IRQ*/
    NVIC_EnableIRQ(INTERRUPT_PRIORITY_NODE_ID);

    /*Generate a load event to start background request source continuous conversion*/
    Cy_VADC_GLOBAL_BackgroundTriggerConversion(VADC);

    while(1);
}
/* [] END OF FILE */
