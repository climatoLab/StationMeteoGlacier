String str_donnees() { //--> Met dans une variable String la structure de nos données
  char vir = ',';
  String s_msg =   String(moSbdMessage.unix) + vir
                 + String(moSbdMessage.Vin) + vir
                 + String(moSbdMessage.bmpTemperatureC) + vir
                 + String(moSbdMessage.bmpPressionHPa) + vir
                 + String(moSbdMessage.bmpAltitude) + vir
                 + String(moSbdMessage.dhtHumidite) + vir
                 + String(moSbdMessage.dhtTemperatureC) + vir
                 + String(moSbdMessage.tcTemperatureC) + vir
                 + String(moSbdMessage.gy49LuminositeLux) + vir
                 + String(moSbdMessage.vlDistanceMM) + vir
                 + String(moSbdMessage.windDirection) + vir
                 + String(moSbdMessage.windSpeed) + "\n";
  return s_msg;
}

// Calculate statistics
//Work in progress
/*
void calculateStats()
{
  // Write data to union
  moSbdMessage.bmpTemperatureC        = bmpTempStats.average()   * 100;                  // Température moyenne BMP388 (°C)
  moSbdMessage.bmpPressionHPa         = (bmpPresStats.average()     - 850) * 100;      // Pression moyenne BMP388 (hPa)
  moSbdMessage.bmpAltitude            = bmpAltStats.average()   * 100;                       // Altitude moyenne BMP388(m)

  // Clear all statistics objects
  clearStats();


}

// Clear statistics objects
void clearStats()
{
  bmpTempStats.clear();
  bmpPresStats.clear();
  bmpAltStats.clear();

}
*/
