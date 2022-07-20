import streamlit as st
import pandas as pd
import snowflake.connector

"""
# Blues Holiday Road Trip Tracker Demo!

This demo pulls data from Snowflake that was routed from [this Notehub project](https://notehub.io/project/app:6e153550-c690-4a7d-ba00-ed6056138574).

The application in question is a Notecard and Notecarrier-F-based asset tracker. In addition to
being configured as a tracker, the Swan-powered host application takes readings from connected
environmental sensors that capture temperature, humidity, altitude, pressure, CO2 and TVOCs.

Raw JSON is routed to Snowflake using the Snowflake SQL API and transformed into
a structured data tables using views, with a view for `_track.qo`, `air.qo`, and `env.qo`
events.

"""

"""
### Options
"""
num_rows = st.slider('Rows to fetch?', 10, 500, 100)
sort = st.selectbox('Sort?',('asc', 'desc'))
show_map = st.checkbox('Show map?', True)
show_charts = st.checkbox('Show charts?', True)
show_table_data = st.checkbox('Show table data?', False)

# Initialize connection.
@st.experimental_singleton
def init_connection():
    return snowflake.connector.connect(**st.secrets["snowflake"])

conn = init_connection()

# Perform query.
@st.experimental_memo(ttl=600)
def run_query(query):
    with conn.cursor() as cur:
        cur.execute(query)
        return cur.fetchall()

tracker_rows = run_query(f'SELECT * from tracker_vw ORDER BY created;')
tracker_data = pd.DataFrame(tracker_rows, columns=("ID", "Device", "When", "Loc", "lat", "lon", "Location", "Location Type", "Timezone", "Country", "Temp", "Voltage"))

air_rows = run_query(f'SELECT * from air_vw ORDER BY created {sort} LIMIT {num_rows};')
air_data = pd.DataFrame(air_rows, columns=("Device", "When", "Loc", "Lat", "Lon", "Location", "Location Type", "Timezone", "Country", "CO2", "TVOC"))

env_rows = run_query(f'SELECT * from env_vw ORDER BY created {sort} LIMIT {num_rows};')

env_data = pd.DataFrame(env_rows, columns=("Device", "When", "Loc", "Lat", "Lon", "Location", "Location Type", "Timezone", "Country", "Temp", "Humidity", "Pressure", "Altitude"))

if show_map:
    """
    ### Tracker Map
    """

    tracker_locations = tracker_data[["lat", "lon"]]

    st.map(tracker_locations)

if show_charts:
    """
    ### Environment Charts
    """

    air_group = air_data[["CO2","TVOC"]]
    st.line_chart(air_group)

    env_group1 = env_data[["Pressure", "Altitude"]]
    st.line_chart(env_group1)

    env_group2 = env_data[["Temp", "Humidity"]]
    st.line_chart(env_group2)

if show_table_data:
    """
    ## Notecard `air.qo` Events
    """

    air_data[air_data.columns[::-1]]

    """
    ## Notecard `env.qo` Events
    """

    env_data[env_data.columns[::-1]]
