import React, { useState } from 'react';
import { Droplets } from 'lucide-react';
import { ref, set } from 'firebase/database';
import { database } from '../lib/firebase';

interface WateringControlProps {
  userId: string;
}

export function WateringControl({ userId }: WateringControlProps) {
  const [isWatering, setIsWatering] = useState(false);

  const handleWatering = async () => {
    try {
      setIsWatering(true);
      // Send watering command to Firebase
      await set(ref(database, `UsersData/${userId}/commands/water`), {
        timestamp: Date.now(),
        action: 'START'
      });

      // Reset after 5 seconds
      setTimeout(async () => {
        await set(ref(database, `UsersData/${userId}/commands/water`), {
          timestamp: Date.now(),
          action: 'STOP'
        });
        setIsWatering(false);
      }, 5000);
    } catch (error) {
      console.error('Error triggering water pump:', error);
      setIsWatering(false);
    }
  };

  return (
    <div className="bg-white rounded-xl shadow-lg p-6">
      <div className="flex flex-col items-center space-y-4">
        <div className="p-4 bg-blue-50 rounded-full">
          <Droplets className="h-8 w-8 text-blue-500" />
        </div>
        <h3 className="text-lg font-semibold">Control del Riego de la Planta</h3>
        <button
          onClick={handleWatering}
          disabled={isWatering}
          className={`px-6 py-3 rounded-lg text-white font-medium transition-colors duration-200 ${
            isWatering
              ? 'bg-blue-400 cursor-not-allowed'
              : 'bg-blue-600 hover:bg-blue-700'
          }`}
        >
          {isWatering ? 'Watering...' : 'Regar'}
        </button>
      </div>
    </div>
  );
}
