
    const MIN = -20, MAX = 50, MID = 15;
    const labels = ['Living Room', 'Kitchen', 'Bedroom', 'Office', 'Garage', 'Basement'];

    //update color picker
    const colorInput = document.getElementById('colorInput');
    const colorPreview = document.getElementById('colorPreview');

    function updatePreview() {
      colorPreview.style.backgroundColor = colorInput.value;
    }

colorInput.addEventListener('input', updatePreview);
updatePreview(); // initialize on load


    // Sidebar toggle
    document.getElementById('menu-toggle').addEventListener('click', () => {
      document.getElementById('sidebar').classList.toggle('open');
    });

    // Page switching
    function showSection(id) {
      document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
      document.getElementById(id).classList.add('active');
      document.getElementById('sidebar').classList.remove('open');
    }

    function colorForTemp(t) {
      if (!Number.isFinite(t)) return '#888';
      const hue = t <= MID
        ? 210 + (50 - 210) * ((t - MIN) / (MID - MIN))
        : 50 + (33 - 50) * ((t - MID) / (MAX - MID));
      return `hsl(${hue}, 100%, 50%)`;
    }

    function getTempsFromURL() {
      const params = new URLSearchParams(window.location.search);
      return Array.from({ length: 6 }, (_, i) => {
        const temp = parseFloat(params.get(`temp${i + 1}`));
        return Number.isFinite(temp) ? temp : null;
      });
    }

    function createGauge(temp, label) {
      const arcPath = "M 43.431,156.569 A 80 80 0 1 1 156.569,156.569";
      const dummy = document.createElementNS("http://www.w3.org/2000/svg", "path");
      dummy.setAttribute("d", arcPath);
      const arcLength = dummy.getTotalLength();
      const offset = Number.isFinite(temp) ? arcLength * (1 - (temp - MIN) / (MAX - MIN)) : arcLength;
      const color = colorForTemp(temp);
      const tempVal = Number.isFinite(temp) ? `${temp}°C` : '--';

      const wrapper = document.createElement("div");
      wrapper.innerHTML = `
        <div class="gauge-label">${label}</div>
        <svg class="circle-bar" viewBox="0 0 200 200" style="width: 100%; max-width: 200px; height: auto;">
          <path class="track" d="${arcPath}" stroke-width="15" />
          <path class="progress" d="${arcPath}" stroke="${color}" stroke-width="15"
            stroke-dasharray="${arcLength}" stroke-dashoffset="${offset}" />
          <text class="tempValue" x="100" y="100" fill="${color}">${tempVal}</text>
        </svg>
      `;
      return wrapper;
    }

    function initHome() {
      const container = document.getElementById('gaugesContainer');
      const temps = getTempsFromURL();
      container.innerHTML = '';
      temps.forEach((temp, i) => {
        container.appendChild(createGauge(temp, labels[i]));
      });
    }

    function initConfig() {
      const container = document.getElementById('formContainer');
      container.innerHTML = '';
      labels.forEach((label, i) => {
        const group = document.createElement('div');
        group.className = 'form-group';
        group.innerHTML = `
          <label for="temp${i}">${label}</label>
          <input type="number" id="temp${i}" placeholder="Enter °C" />
        `;
        container.appendChild(group);
      });
    }

async function writeToNFC() {
  try {
    const header = '<TEMP>';
    let payload = '';

    for (let i = 0; i < 6; i++) {
      const input = document.getElementById(`temp${i}`);
      let mapped = 0; // Default to 0x00 for missing/invalid input

      if (input) {
        const val = parseFloat(input.value);

        if (Number.isFinite(val)) {
          const clamped = Math.max(-20, Math.min(50, val));
          mapped = Math.round((clamped + 20) * 255 / 70);
        }
      }

      // Append 2-digit uppercase hex
      payload += mapped.toString(16).padStart(2, '0').toUpperCase();
    }

    const fullText = header + payload;

    if (!('NDEFReader' in window)) {
      alert('Web NFC unsupported. Use a compatible Android device and HTTPS.');
      return;
    }

    const ndef = new NDEFReader();
    await ndef.write({
      records: [
        {
          recordType: "text",
          data: fullText
        }
      ]
    });

    const btn = document.getElementById('writeBtn');
    btn.textContent = '✅ NFC write success';
    setTimeout(() => btn.textContent = 'Write to NFC', 2000);
  } catch (err) {
    alert('❌ NFC write failed. Make sure your device supports Web NFC and uses HTTPS.');
    console.error(err);
  }
}
 

    document.addEventListener('DOMContentLoaded', () => {
      initHome();
      initConfig();
      document.getElementById('writeBtn').addEventListener('click', writeToNFC);
    });

  if ('serviceWorker' in navigator) {
    navigator.serviceWorker.register('/service-worker.js')
      .then(() => console.log('Service Worker registered'))
      .catch(err => console.error('Service Worker registration failed:', err));
  }




document.getElementById('colorOkBtn').addEventListener('click', async () => {
  try {
    const colorHex = document.getElementById('colorInput').value; // "#00FF00"
    console.log("Color input value:", colorHex);

    // Convert hex to RGB
    const r = parseInt(colorHex.substring(1, 3), 16);
    const g = parseInt(colorHex.substring(3, 5), 16);
    const b = parseInt(colorHex.substring(5, 7), 16);

    const rgbBytes = new Uint8Array([r, g, b]);

    const ndef = new NDEFWriter();
    await ndef.write({
      records: [{
        recordType: "mime",
        mediaType: "application/octet-stream",
        data: rgbBytes
      }]
    });

    alert(`✅ Color ${colorHex} written to NFC tag as RGB(${r}, ${g}, ${b})`);
  } catch (error) {
    console.error("❌ NFC write failed:", error);
    alert("Failed to write to NFC tag. Make sure you're using Chrome on Android with NFC enabled.");
  }
})


