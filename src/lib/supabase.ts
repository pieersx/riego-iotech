import { createClient } from '@supabase/supabase-js';

const supabaseUrl = '';     // databaseURL
const supabaseKey = '';     // apiKey

export const supabase = createClient(supabaseUrl, supabaseKey);
