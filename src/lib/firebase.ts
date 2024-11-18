import { initializeApp } from 'firebase/app';
import { getAuth, GoogleAuthProvider } from 'firebase/auth';
import { getDatabase } from 'firebase/database';

const firebaseConfig = {
  apiKey: "AIzaSyBdQDh-VkRQGcT_7T5ac9XvFVoeT0KH3UY",
  authDomain: "riego-iotech.firebaseapp.com",
  databaseURL: "https://riego-iotech-default-rtdb.firebaseio.com",
  projectId: "riego-iotech",
  storageBucket: "riego-iotech.firebasestorage.app",
  messagingSenderId: "163534901539",
  appId: "1:163534901539:web:ce4eb6d548a1d2d5c47c96",
  measurementId: "G-SB5FTGJLY8"
};

const app = initializeApp(firebaseConfig);
export const auth = getAuth(app);
export const database = getDatabase(app);
export const googleProvider = new GoogleAuthProvider();
