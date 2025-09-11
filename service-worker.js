const CACHE_NAME = 'html-shell-cache-v1';
const SHELL_URL = '/index.html';

// Install event: cache the shell
self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME).then(cache => cache.add(SHELL_URL))
  );
  self.skipWaiting(); // Activate immediately
});

// Activate event: enable navigation preload
self.addEventListener('activate', event => {
  event.waitUntil(self.registration.navigationPreload.enable());
  self.clients.claim(); // Take control of all pages
});

// Fetch event: intercept navigation requests
self.addEventListener('fetch', event => {
  if (event.request.mode === 'navigate') {
    event.respondWith(
      (async () => {
        // Try preload response first
        const preloadResponse = await event.preloadResponse;
        if (preloadResponse) return preloadResponse;

        // Try cached shell
        const cachedShell = await caches.match(SHELL_URL);
        if (cachedShell) return cachedShell;

        // Fallback to network
        return fetch(SHELL_URL);
      })()
    );
  }
});