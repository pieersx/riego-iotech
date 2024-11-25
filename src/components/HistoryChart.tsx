import React from 'react';
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer
} from 'recharts';

interface HistoryChartProps {
  data: Array<{ value: number; timestamp: number }>;
  title: string;
  unit: string;
  color: string;
}

export function HistoryChart({ data, title, unit, color }: HistoryChartProps) {
  const formatDate = (timestamp: number) => {
    return new Date(timestamp).toLocaleTimeString();
  };

  const formatValue = (value: number) => `${value.toFixed(1)}${unit}`;

  return (
    <div className="bg-white rounded-xl shadow-sm border border-gray-200 p-6 hover:shadow-md transition-shadow duration-200">
      <h3 className="text-lg font-semibold text-gray-900 mb-4">{title}</h3>
      <div className="h-[300px]">
        <ResponsiveContainer width="100%" height="100%">
          <LineChart data={data} margin={{ top: 5, right: 30, left: 20, bottom: 5 }}>
            <CartesianGrid strokeDasharray="3 3" stroke="#e5e7eb" />
            <XAxis
              dataKey="timestamp"
              tickFormatter={formatDate}
              stroke="#6b7280"
            />
            <YAxis
              stroke="#6b7280"
              tickFormatter={(value) => value.toFixed(1)}
            />
            <Tooltip
              contentStyle={{
                backgroundColor: 'white',
                border: '1px solid #e5e7eb',
                borderRadius: '0.5rem',
                boxShadow: '0 2px 4px rgba(0,0,0,0.1)'
              }}
              labelFormatter={formatDate}
              formatter={(value: number) => [formatValue(value), '']}
            />
            <Line
              type="monotone"
              dataKey="value"
              stroke={color}
              strokeWidth={2}
              dot={false}
              activeDot={{ r: 4 }}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
