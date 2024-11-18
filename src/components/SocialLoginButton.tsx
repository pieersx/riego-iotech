import React from 'react';
import { LucideIcon } from 'lucide-react';

interface SocialLoginButtonProps {
  icon: LucideIcon;
  onClick: () => void;
  label: string;
  provider: string;
}

export function SocialLoginButton({ icon: Icon, onClick, label, provider }: SocialLoginButtonProps) {
  return (
    <button
      type="button"
      onClick={onClick}
      className="w-full flex items-center justify-center gap-2 px-4 py-2 border border-gray-300 rounded-md shadow-sm text-sm font-medium text-gray-700 bg-white hover:bg-gray-50 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500 transition-colors duration-200"
    >
      <Icon className="h-5 w-5" aria-hidden="true" />
      <span>{label}</span>
    </button>
  );
}
