import { getLocalIPAddress } from '@/app/utils/networkUtils';

export const sendRequest = async (endpoint) => {
	const controller = new AbortController();
	const timeoutId = setTimeout(() => controller.abort(), 1000); // Abort after 1 second

	try {
		const baseUrl = await getLocalIPAddress();
		fetch(`${baseUrl}${endpoint}`, {
			method: 'GET',
			signal: controller.signal,
		}).catch(() => {
			// Ignore any errors - we don't want to block on failed requests
		});
	} catch (err) {
	} finally {
		clearTimeout(timeoutId);
	}
};
