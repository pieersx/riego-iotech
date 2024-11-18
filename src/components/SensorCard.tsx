import React from 'react';
import { Card } from 'lucide-react';

interface SensorCardProps {
  title: string;
  value: number | string;
  unit: string;
  icon: React.ReactNode;
}

export function SensorCard({ title, value, unit, icon }: SensorCardProps) {
  return (
    <div className="bg-white rounded-xl shadow-lg p-6 flex items-center space-x-4">
      <div className="p-3 bg-blue-50 rounded-lg">
        {icon}
      </div>
      <div>
        <h3 className="text-gray-500 text-sm font-medium">{title}</h3>
        <p className="text-2xl font-semibold text-gray-900">
          {value} <span className="text-sm text-gray-500">{unit}</span>
        </p>
      </div>
    </div>
  );
}
