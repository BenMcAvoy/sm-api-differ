// Cloudflare Worker: proxies /counter/* on the site's own domain through to
// GoatCounter, so the analytics beacon is same-origin and isn't caught by
// domain-based ad-block/tracker-protection lists that target *.goatcounter.com.
//
// Deploy: Cloudflare dashboard -> Workers & Pages -> Create Worker -> paste
// this in (Quick Edit), then add a route under the Worker's Triggers tab:
//   sm-api-differ.benmcavoy.co.uk/counter/*
//
// Not tied to any repo build step -- this file is just the source of truth
// for what's pasted into the dashboard.

const GOATCOUNTER_HOST = "https://sm-api-differ.goatcounter.com";

export default {
    async fetch(request) {
        const url = new URL(request.url);

        if (!url.pathname.startsWith("/counter/")) {
            return new Response("Not found", { status: 404 });
        }

        const targetPath = url.pathname.replace(/^\/counter/, "");
        const targetUrl = GOATCOUNTER_HOST + targetPath + url.search;

        const upstream = await fetch(targetUrl, {
            method: request.method,
            headers: request.headers,
            body: ["GET", "HEAD"].includes(request.method) ? undefined : request.body,
        });

        const response = new Response(upstream.body, upstream);
        response.headers.set("Access-Control-Allow-Origin", "*");
        return response;
    },
};
