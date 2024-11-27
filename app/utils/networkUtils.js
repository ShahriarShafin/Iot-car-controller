'use client';
export const getLocalIPAddress = () => {
	return new Promise((resolve, reject) => {
		const RTCPeerConnection =
			window.RTCPeerConnection || window.webkitRTCPeerConnection;
		const pc = new RTCPeerConnection({ iceServers: [] });
		const noop = () => {};
		pc.createDataChannel(''); // Create a bogus data channel

		pc.createOffer(pc.setLocalDescription.bind(pc), noop);

		pc.onicecandidate = (ice) => {
			// if (!ice || !ice.candidate || !ice.candidate.candidate) return;
			// const ip = /([0-9]{1,3}(\.[0-9]{1,3}){3})/.exec(
			// 	ice.candidate.candidate
			// )[1];
			// // console.log('Local IP Address:', ip);

			// // Adjust the BASE_URL based on the detected IP
			// if (ip.startsWith('192.168.214.')) {
			// 	resolve('http://192.168.214.106');
			// } else if (ip.startsWith('192.168.4.')) {
			// 	resolve('http://192.168.4.1');
			// } else {
			// 	resolve(`http://${ip}`); // Fallback to the detected IP
			// }
			resolve('http://192.168.4.1');
			// resolve('http://192.168.0.222');

			pc.onicecandidate = noop;
		};
		setTimeout(() => {
			reject('Failed to detect local IP address');
		}, 5000); // Set a timeout in case of failure
	});
};
