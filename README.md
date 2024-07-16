# Smart-Farming-using-IoT

This project is a Smart Farming system that leverages IoT technology to automate irrigation based on real-time weather conditions and soil moisture levels. The system is designed to optimize water usage and improve crop management by making intelligent decisions.

## Features

- **Weather Forecast Integration**: Retrieves weather data for the next 3 hours using an API to determine if it will rain.
- **Soil Moisture Monitoring**: Continuously checks soil moisture levels to assess the need for irrigation.
- **Automated Irrigation Control**: Uses a servo motor to control the irrigation system. If it's not raining and the soil is dry, the system will automatically turn on the irrigation.
- **Efficiency**: Ensures water is used only when necessary, preventing over-irrigation and conserving resources.

## Components

- IoT Circuit
- Soil Moisture Sensor
- Servo Motor
- Weather API

## How It Works

1. **Weather Data Retrieval**: The system fetches weather forecasts for the next 3 hours.
2. **Decision Making**: If rain is predicted, the system will not activate the irrigation, regardless of soil moisture levels.
3. **Soil Moisture Check**: If no rain is forecasted, the system checks the soil moisture sensor.
4. **Irrigation Control**: If the soil is dry and no rain is expected, the servo motor activates to turn on the irrigation system.

This project demonstrates the potential of IoT in smart agriculture, providing a scalable solution for efficient water management in farming.
