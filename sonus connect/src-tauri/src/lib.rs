use std::io::Write;
use std::process::Command;
use std::path::Path;

// Learn more about Tauri commands at https://tauri.app/develop/calling-rust/
#[tauri::command]
fn greet(name: &str) -> String {
    format!("Hello, {}! You've been greeted from Rust!", name)
}

#[tauri::command]
fn scan_for_drive() -> Option<String> {
    for drive in b'D'..=b'Z' {
        let path = format!("{}:\\sdir.txt", drive as char);
        if std::path::Path::new(&path).exists() {
            return Some(format!("{}:\\", drive as char));
        }
    }
    None
}

#[tauri::command]
fn verify_drive(path: &str) -> bool {
    std::path::Path::new(path).join("sdir.txt").exists()
}

#[tauri::command]
async fn process_audio(
    title: String,
    author: String,
    file_name: String,
    file_data: Vec<u8>,
    mount_point: String,
) -> Result<(), String> {
    let temp_dir = std::env::temp_dir();
    let temp_input = temp_dir.join(&file_name);
    
    std::fs::write(&temp_input, file_data).map_err(|e| format!("Failed to write temp file: {}", e))?;
    
    let raw_file_name = Path::new(&file_name)
        .with_extension("raw")
        .file_name()
        .unwrap()
        .to_string_lossy()
        .into_owned();
        
    let output_path = Path::new(&mount_point).join(&raw_file_name);
    
    let output = Command::new("ffmpeg")
        .arg("-y")
        .arg("-i")
        .arg(&temp_input)
        .arg("-ar")
        .arg("44100")
        .arg("-acodec")
        .arg("pcm_s16le")
        .arg("-f")
        .arg("s16le")
        .arg(&output_path)
        .output()
        .map_err(|e| format!("Failed to execute ffmpeg: {}", e))?;
        
    let stderr = String::from_utf8_lossy(&output.stderr);
    
    if !output.status.success() {
        let _ = std::fs::remove_file(&temp_input);
        return Err(format!("ffmpeg failed: {}", stderr));
    }
    
    let mut duration_str = "0:00".to_string();
    if let Some(duration_idx) = stderr.find("Duration: ") {
        let start = duration_idx + 10;
        let end = stderr[start..].find(',').unwrap_or(stderr.len() - start);
        let duration_full = &stderr[start..start + end];
        let parts: Vec<&str> = duration_full.split(':').collect();
        if parts.len() == 3 {
            let hours: u32 = parts[0].trim().parse().unwrap_or(0);
            let minutes: u32 = parts[1].trim().parse().unwrap_or(0);
            let seconds_full: f32 = parts[2].trim().parse().unwrap_or(0.0);
            let seconds = seconds_full as u32;
            let total_minutes = hours * 60 + minutes;
            duration_str = format!("{}:{:02}", total_minutes, seconds);
        }
    }
    
    let sdir_path = Path::new(&mount_point).join("sdir.txt");
    let sdir_content = std::fs::read_to_string(&sdir_path).unwrap_or_default();
    
    let mut max_id = 0;
    for item in sdir_content.split(';') {
        let item = item.trim();
        if item.is_empty() {
            continue;
        }
        if let Some(dash_idx) = item.find('-') {
            if let Ok(id) = item[..dash_idx].parse::<u32>() {
                if id > max_id {
                    max_id = id;
                }
            }
        }
    }
    
    let new_id = max_id + 1;
    let new_entry = format!("{}-{}={}+{}>{};\n", new_id, title, author, raw_file_name, duration_str);
    
    std::fs::OpenOptions::new()
        .create(true)
        .append(true)
        .open(&sdir_path)
        .map_err(|e| format!("Failed to open sdir.txt: {}", e))?
        .write_all(new_entry.as_bytes())
        .map_err(|e| format!("Failed to write to sdir.txt: {}", e))?;
        
    let _ = std::fs::remove_file(&temp_input);
    
    Ok(())
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_dialog::init())
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![greet, scan_for_drive, process_audio, verify_drive])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
