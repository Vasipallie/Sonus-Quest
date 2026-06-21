import { invoke } from "@tauri-apps/api/core";
import { open } from "@tauri-apps/plugin-dialog";

async function initializeDrive() {
  let currentDrive = localStorage.getItem('selectedDrive');

  if (currentDrive) {
    try {
      const isValid = await invoke<boolean>("verify_drive", { path: currentDrive });
      if (!isValid) {
        currentDrive = null;
        localStorage.removeItem('selectedDrive');
      }
    } catch (e) {
      currentDrive = null;
    }
  }

  if (!currentDrive) {
    try {
      currentDrive = await invoke<string | null>("scan_for_drive");
      if (currentDrive) {
        localStorage.setItem('selectedDrive', currentDrive);
      }
    } catch (e) {
      console.error("Failed to scan for drive:", e);
    }
  }

  function updateDisplay(drive: string | null) {
    const statusHeader = document.querySelector('.imrgh');
    const deviceNameP = document.querySelector('.imrgs:not(.a)');
    const mountPointP = document.querySelector('.imrgs.a');
    
    if (statusHeader) {
      statusHeader.textContent = drive ? "Connected" : "Not Connected";
    }
    
    if (deviceNameP) {
      deviceNameP.innerHTML = drive ? `to <span id="device-name">Sonus Quest φ</span>` : `Please mount Sonus Quest φ`;
    }
    
    if (mountPointP) {
      mountPointP.innerHTML = drive ? `mounted on <span id="mount-point">${drive}</span> <span style="font-size: 0.8em; text-decoration: underline; margin-left: 8px;">(Change)</span>` : `<span style="text-decoration: underline;">Click here to select drive</span>`;
    }
  }

  updateDisplay(currentDrive);

  const driveLinks = document.querySelectorAll('.imrgs.a');
  driveLinks.forEach(link => {
    if (link.tagName !== 'A') {
      (link as HTMLElement).style.cursor = 'pointer';
    }
    
    link.addEventListener('click', async (e) => {
      e.preventDefault(); 
      try {
        const selected = await open({
          directory: true,
          multiple: false,
          title: "Select Sonus Quest Drive"
        });
        
        if (selected) {
          const newDrive = selected as string;
          const isValid = await invoke<boolean>("verify_drive", { path: newDrive });
          if (isValid) {
            currentDrive = newDrive;
            localStorage.setItem('selectedDrive', currentDrive);
            updateDisplay(currentDrive);
          } else {
            alert("The selected directory does not appear to be a valid Sonus Quest drive (missing sdir.txt).");
          }
        }
      } catch (e) {
        console.error("Failed to open dialog:", e);
      }
    });
  });
}

document.addEventListener("DOMContentLoaded", () => {
  initializeDrive();

  const form = document.querySelector('form');
  if (form) {
    form.addEventListener('submit', async (e) => {
      e.preventDefault();
      
      const titleInput = document.getElementById('title') as HTMLInputElement;
      const authorInput = document.getElementById('author') as HTMLInputElement;
      const fileInput = document.getElementById('file') as HTMLInputElement;
      const title = titleInput.value;
      const author = authorInput.value;
      const file = fileInput.files?.[0];
      const mountPoint = localStorage.getItem('selectedDrive');
      
      if (!file) {
        alert("Please select a file to upload");
        return;
      }
      
      if (!mountPoint) {
        alert("Please select a Sonus Quest drive first");
        return;
      }
      
      try {
        const submitBtn = document.querySelector('.submit-btn') as HTMLButtonElement;
        const originalText = submitBtn.textContent;
        submitBtn.textContent = "Processing...";
        submitBtn.disabled = true;
        
        const arrayBuffer = await file.arrayBuffer();
        await invoke("process_audio", {
          title,
          author,
          fileName: file.name,
          fileData: Array.from(new Uint8Array(arrayBuffer)),
          mountPoint
        });        
        alert("Upload successful!");
        form.reset();
        submitBtn.textContent = originalText;
        submitBtn.disabled = false;
      } catch (error) {
        console.error(error);
        alert("Failed to process audio: " + error);
        const submitBtn = document.querySelector('.submit-btn') as HTMLButtonElement;
        submitBtn.textContent = "Upload Song";
        submitBtn.disabled = false;
      }
    });
  }
});
