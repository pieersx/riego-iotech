import React from "react";
import { Loader2 } from "lucide-react";

export function LoadingScreen() {
    return (
        <div className="min-h-screen flex items-center justify-center bg-gradient-to-br from-blue-50 to-indigo-50">
          <div className="flex flex-col items-center space-y-4">
            <Loader2 className="h-8 w-8 text-blue-600 animate-spin" />
            <p className="text-gray-600 font-medium">Loading...</p>
          </div>
        </div>
    );
}
