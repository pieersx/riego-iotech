import { createClient } from '@supabase/supabase-js';

const supabaseUrl = 'https://riego-iotech-default-rtdb.firebaseio.com/';
const supabaseKey = 'AIzaSyBdQDh-VkRQGcT_7T5ac9XvFVoeT0KH3UY';

export const supabase = createClient(supabaseUrl, supabaseKey);
