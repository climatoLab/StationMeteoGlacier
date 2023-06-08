// Calculate statistics
//Work in progress

void calculateStats()
{
  // Write data to union
  moSbdMessage.bmpTemperatureC        = bmpTempStats.average()   * 100;                  // Température moyenne BMP388 (°C)
  moSbdMessage.bmpPressionHPa         = bmpPresStats.average();                         // Pression moyenne BMP388 (hPa)
  moSbdMessage.bmpAltitude            = bmpAltStats.average();                           // Altitude moyenne BMP388(m)
  moSbdMessage.dhtTemperatureC        = dhtTempStats.average() * 100;          
  moSbdMessage.dhtHumidite            = dhtHumStats.average();           
  moSbdMessage.tcTemperatureC         = tcTempStats.average() * 100;           
  moSbdMessage.gy49LuminositeLux      = gyLuxStats.average();            
  moSbdMessage.vlDistanceMM           = vlDisStats.average();            
  moSbdMessage.windDirection          = windDirectionStats.average();    
  moSbdMessage.windSpeed              = windSpeedStats.average();        
  moSbdMessage.Vin                    = voltageStats.average() * 100;          
  // Clear all statistics objects
  clearStats();


}

// Clear statistics objects
void clearStats()
{
  bmpTempStats.clear();
  bmpPresStats.clear();
  bmpAltStats.clear();
  dhtTempStats.clear();          
  dhtHumStats.clear();           
  tcTempStats.clear();           
  gyLuxStats.clear();            
  vlDisStats.clear();            
  windDirectionStats.clear();    
  windSpeedStats.clear();        
  voltageStats.clear();   

}
