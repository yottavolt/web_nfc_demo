const CACHE_NAME = 'html-shell-cache-v1';
const SHELL_URL = '/index.html'; // Adjust if your main file is named differently

self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME).then(cache => cache.add(SHELL_URL))
  );
});

self.addEventListener('fetch', event => {
  if (event.request.mode === 'navigate') {
    event.respondWith(
      caches.match(SHELL_URL).then(response => response || fetch(SHELL_URL))
    );
  }
});