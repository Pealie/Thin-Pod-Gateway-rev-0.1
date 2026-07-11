# DWM3001-CDK Host Interface Stub

Runs on the Gateway DWM3001-CDK.

Purpose:
- Treat the DWM3001-CDK as an intelligent UWB coprocessor/module.
- Provide the first DWM-to-NUCLEO host-interface proof.
- Answer GET_CAPABILITIES, GET_STATUS and GET_COUNTERS.

Initial role values:
- role = gateway_initiator
- backend = stub
- state = ready

This proof does not claim direct NUCLEO ownership of the DW3110 silicon.
