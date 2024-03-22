import React, { useCallback, useEffect, useState } from "react";
import ReadingService from "../../services/ReadingService";
import Readings from "../../models/Readings";
import DashboardAllReadings from "./DashboardAllReadings";

const DashboardReadings: React.FC = () => {
    const [dataSource, setDataSource] = useState<Readings[] | null>(null);
    const fetchData = useCallback(async () => {
      try {
        let result = await ReadingService.getReadings();
        result?.map(r => {r.type = r.sensor_id + " " + r.type})
        setDataSource(result ?? null);
        console.log(dataSource)
      } catch (error) {
        setDataSource(null);
      }
    }, []);
  
    useEffect(() => {
      fetchData();
    }, [fetchData]);
  
    
    return <div style={{height:"80vh"}}>{dataSource && <DashboardAllReadings readings={dataSource}></DashboardAllReadings>}</div>
}

export default DashboardReadings;