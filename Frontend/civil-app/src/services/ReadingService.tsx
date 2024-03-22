import React from "react";
import axios, { AxiosResponse } from 'axios';
import { API_URL } from "../routes/AppRoutes";
import Readings from "../models/Readings";

const API_LOCALE = "/api/readings"

const ReadingService = {
    async getReadings(): Promise<Readings[] | null> {
        try {
          const response: AxiosResponse<Readings[]> = await axios.get(`${API_URL + API_LOCALE}`);
          return response.data;
        } catch (error) {
          return null;
        }
      },
}

export default ReadingService