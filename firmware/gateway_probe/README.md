\# Thin-Pod Gateway SPI5/GPIO Probe App



This minimal firmware app supports Gateway completion checklist item 3: minimal SPI/GPIO probe app.



The purpose is to verify that the Thin-Pod Gateway rev 0.1 PCB allows firmware-level control of the DWM3001-CDK interface pins.



This app is intentionally limited to:



\- boot banner output;

\- DWM reset GPIO toggle;

\- DWM chip-select GPIO toggle;

\- DWM IRQ GPIO read;

\- minimal SPI5 transmit activity.



This app is not a DW3110 driver and does not claim full UWB operation.



\## Expected probe evidence



A successful probe should produce evidence that:



\- firmware starts successfully;

\- DWM\_RESET can be driven low and high;

\- DWM\_CS can be driven low and high;

\- DWM\_IRQ can be sampled as an input;

\- SPI5 can transmit a short test frame while CS is asserted.



\## Completion checklist link



This app directly supports:



\- Item 3: minimal SPI/GPIO probe app



It also supports:



\- Item 2: final verified pin map

\- Item 7: validation log

