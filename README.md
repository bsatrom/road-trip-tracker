# Holiday Road Trip Tracker

A Cellular IoT and GPS/GNSS tracker that measures air quality using SparkFun
Qwiic sensors, the Blues Swan, and Blues Notecard. Data is routed from the
Blues [Notehub](https://notehub.io) to [Snowflake](https://snowflake.com)
where it is then fetched and visualized using [Streamlit](https://streamlit.io).

The full project writeup is [on Hackster.io](https://hackster.io/blues-wireless).

This repository contains the complete source for this project, including:

- [Firmware](firmware/src/main.cpp) (Using Platform.io) for the Blues Swan connected to a Notecard via the [Notecarrier-F](https://shop.blues.io/products/carr-f).
- A [Streamlit.io](https://streamlit.io) [application](dashboard/streamlit_app.py) for fetching data from Snowflake and displaying charts, graphs, and a map.
