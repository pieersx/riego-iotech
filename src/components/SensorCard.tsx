import React from "react";

interface SensorCardProps {
  title: string;
  value: number | string;
  unit: string;
  icon: React.ReactNode;
  description: string;
  status?: "normal" | "warning" | "alert";
}

export function SensorCard({
  title,
  value,
  unit,
  icon,
  description,
  status = "normal",
}: SensorCardProps) {
  const getStatusColor = () => {
    switch (status) {
      case "warning":
        return "bg-yellow-50 text-yellow-600 border-yellow-200";
      case "alert":
        return "bg-red-50 text-red-600 border-red-200";
      default:
        return "bg-blue-50 text-blue-600 border-blue-200";
    }
  };

  return (
    <div className="bg-white rounded-xl shadow-sm border border-gray-200 p-6 hover:shadow-md transition-shadow duration-200">
      <div className="flex items-center space-x-4">
        <div className={`p-3 rounded-lg border ${getStatusColor()}`}>
          {icon}
        </div>
        <div>
          <h3 className="text-sm font-medium text-gray-500">{title}</h3>
          <p className="text-2xl font-semibold text-gray-900">
            {value} <span className="text-sm text-gray-500">{unit}</span>
          </p>
          {description && (
            <p className="text-sm mt-1 text-gray-500">{description}</p>
          )}
        </div>
      </div>
    </div>
  );
}
