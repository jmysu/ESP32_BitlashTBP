# ESP32_BitlashTBP
### ESP32 project with QEspNow &amp; Bitlash &amp; TinyBasicPlus


Script interpreters inside: <br>
1) Bitlash ESP32 V2.1j w/ QEspNOW Wireless<br>
2) TinyBasicPlus V0.15j <br>
3) LittleFS script workaround V0228 <br> 
   
<img src="BitlashTBP.png" width="48%"> <img src="BitlashTBP_Depends.png" width="50%"> <br>
<br>
The doCommand() is not re-entrant! Need some workarounds for executing LittleFS scripts...<br>
<img src="BitlashFS0228.png" width="75%"> <br>

---
## References
  - [Bitlash](http://bitlash.net/) A programmable command shell for arduino. <br>
  - [TinyBasic+](https://github.com/BleuLlama/TinyBasicPlus) A C implementation of Tiny Basic. <br>
  - [TinyBasic WiKi](https://en.wikipedia.org/wiki/Tiny_BASIC) TinyBasic Wikipedia page. <br>
  - [Quick EspNOW](https://github.com/gmag11/QuickESPNow) A wireless protocol that allows devices links without network.
