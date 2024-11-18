import React, { useEffect, useState } from 'react';
import { Thermometer, Droplets, Wind, Activity, LogOut } from 'lucide-react';
import { ref, onValue } from 'firebase/database';
import { signOut, onAuthStateChanged } from 'firebase/auth';
import { auth, database } from './lib/firebase';
import { LoginForm } from './components/LoginForm';
import { SensorCard } from './components/SensorCard';
import { Chart } from './components/Chart';
import { WateringControl } from './components/WateringControl';

interface SensorData {
  temperature: number;
  humidity: number;
  pressure: number;
}

function App() {
  const [user, setUser] = useState(auth.currentUser);
  const [sensorData, setSensorData] = useState<SensorData>({
    temperature: 0,
    humidity: 0,
    pressure: 0
  });
  const [historicalData, setHistoricalData] = useState([]);

  useEffect(() => {
    const unsubscribe = onAuthStateChanged(auth, (user) => {
      setUser(user);
      if (user) {
        // Subscribe to real-time updates
        const userDataRef = ref(database, `UsersData/${user.uid}`);
        onValue(userDataRef, (snapshot) => {
          const data = snapshot.val();
          if (data) {
            setSensorData({
              temperature: data.temperature || 0,
              humidity: data.humidity || 0,
              pressure: data.pressure || 0
            });
          }
        });
      }
    });

    return () => unsubscribe();
  }, []);

  const handleLogout = async () => {
    try {
      await signOut(auth);
    } catch (error) {
      console.error('Error signing out:', error);
    }
  };

  if (!user) {
    return <LoginForm onLogin={() => {}} />;
  }

  return (
    <div className="min-h-screen bg-gray-50">
      <nav className="bg-white shadow-sm">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between h-16 items-center">
            <div className="flex items-center">
              <Activity className="h-8 w-8 text-blue-500" />
              <span className="ml-2 text-xl font-semibold">ESP32 Dashboard</span>
            </div>
            <div className="flex items-center space-x-4">
              <span className="text-gray-600">{user.email}</span>
              <button
                onClick={handleLogout}
                className="inline-flex items-center px-3 py-2 border border-transparent text-sm leading-4 font-medium rounded-md text-white bg-blue-600 hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500"
              >
                <LogOut className="h-4 w-4 mr-2" />
                Logout
              </button>
            </div>
          </div>
        </div>
      </nav>

      <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8">
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mb-8">
          <SensorCard
            title="Temperatura"
            value={sensorData.temperature.toFixed(1)}
            unit="°C"
            icon={<Thermometer className="h-6 w-6 text-blue-500" />}
          />
          <SensorCard
            title="Humedad"
            value={sensorData.humidity.toFixed(1)}
            unit="%"
            icon={<Droplets className="h-6 w-6 text-blue-500" />}
          />
          <SensorCard
            title="Presión"
            value={sensorData.pressure.toFixed(0)}
            unit="hPa"
            icon={<Wind className="h-6 w-6 text-blue-500" />}
          />
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mb-8">
          <div className="lg:col-span-2">
            <Chart
              data={historicalData}
              dataKey="temperature"
              title="Historial de Temperatura"
            />
          </div>
          <WateringControl userId={user.uid} />
        </div>
      </main>
    </div>
  );
}

export default App;
