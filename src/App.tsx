import { useEffect, useState } from "react";
import { Thermometer,  Droplets, LogOut,  Sprout } from "lucide-react";
import { ref, onValue, query, orderByChild, limitToLast } from "firebase/database";
import { signOut, onAuthStateChanged } from "firebase/auth";
import { auth, database } from "./lib/firebase";
import { LoginForm } from "./components/LoginForm";
import { SensorCard } from "./components/SensorCard";
import { WateringControl } from "./components/WateringControl";
import { HistoryChart } from "./components/HistoryChart";

interface SensorData {
  temperature: number;
  humidity: number;
  soilMoisture: number;
  timestamp: number;
}

interface HistoricalData {
  temperature: Array<{ value: number; timestamp: number }>;
  humidity: Array<{ value: number; timestamp: number }>;
  soilMoisture: Array<{ value: number; timestamp: number }>;
}

function App() {
  const [user, setUser] = useState(auth.currentUser);
  const [sensorData, setSensorData] = useState<SensorData>({
    temperature: 0,
    humidity: 0,
    soilMoisture: 0,
    timestamp: 0
  });
  const [historicalData, setHistoricalData] = useState<HistoricalData>({
    temperature: [],
    humidity: [],
    soilMoisture: []
  });

  useEffect(() => {
    const unsubscribe = onAuthStateChanged(auth, (user) => {
      setUser(user);
      if (user) {
        // Subscribe to real-time updates
        const userDataRef = ref(database, `UsersData/${user.uid}/current`);
        onValue(userDataRef, (snapshot) => {
          const data = snapshot.val();
          if (data) {
            setSensorData({
              temperature: data.temperature || 0,
              humidity: data.humidity || 0,
              soilMoisture: data.soilMoisture || 0,
              timestamp: data.timestamp || 0
            });
          }
        });

        const historyRef = query(
          ref(database, `UsersData/${user.uid}/history`),
          orderByChild('timestamp'),
          limitToLast(50)
        );

        onValue(historyRef, (snapshot) => {
          const data = snapshot.val();
          if (data) {
            const entries = Object.values(data) as Array<SensorData>;
            setHistoricalData({
              temperature: entries.map(entry => ({
                value: entry.temperature,
                timestamp: entry.timestamp
              })),
              humidity: entries.map(entry => ({
                value: entry.humidity,
                timestamp: entry.timestamp
              })),
              soilMoisture: entries.map(entry => ({
                value: entry.soilMoisture,
                timestamp: entry.timestamp
              }))
            });
          }
        });
      }
    });

    return () => unsubscribe();
  }, []);

  const getSoilMoistureStatus = (moisture: number) => {
    if (moisture < 30) return 'alert';
    if (moisture < 50) return 'warning'
    return 'normal';
  }

  const handleLogout = async () => {
    try {
      await signOut(auth);
    } catch (error) {
      console.error("Error signing out:", error);
    }
  };

  if (!user) {
    return <LoginForm onLogin={() => {}} />;
  }

  return (
    <div className="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-50">
      <nav className="bg-white border-b border-gray-200 sticky top-0 z-50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between h-16 items-center">
            <div className="flex items-center">
              <Sprout className="h-8 w-8 text-green-600" />
              <span className="ml-2 text-xl font-semibold text-gray-900">
                Plant Monitor
              </span>
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

      <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8 space-y-8">
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          <SensorCard
            title="Temperatura"
            value={sensorData.temperature.toFixed(1)}
            unit="°C"
            icon={<Thermometer className="h-6 w-6" />}
            description="Temperatura Ambiente"
          />
          <SensorCard
            title="Humedad del Aire"
            value={sensorData.humidity.toFixed(1)}
            unit="%"
            icon={<Droplets className="h-6 w-6" />}
            description="Humedad relativa"
          />
          <SensorCard
            title="Humedad del Suelo"
            value={sensorData.soilMoisture.toFixed(1)}
            unit="%"
            icon={<Sprout className="h-6 w-6" />}
            status={getSoilMoistureStatus(sensorData.soilMoisture)}
            description={
              sensorData.soilMoisture < 30
                ? '¡El suelo está demasiado seco!'
                : sensorData.soilMoisture > 70
                ? 'El suelo está bien regado'
                : 'La humedad del suelo es adecuada'
            }
          />
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <HistoryChart
            data={historicalData.temperature}
            title="Historial de Temperatura"
            unit="°C"
            color="#2563eb"
          />
          <HistoryChart
            data={historicalData.humidity}
            title="Historial de Humedad del Aire"
            unit="%"
            color="#0891b2"
          />
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <HistoryChart
            data={historicalData.soilMoisture}
            title="Historial de Humedad del Suelo"
            unit="%"
            color="#059669"
          />
          <WateringControl userId={user.uid} />
        </div>
      </main>
    </div>
  );
}

export default App;
