// AWS Certificates

#include "aws_certificates.h"

/*
 * PEM-encoded root CA certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
const char AWS_RootCA[] = "Paste root CA certificate here.";

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
const char AWS_ClientCert[] = "Paste client certificate here.";

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----"
 * "...base64 data..."
 * "-----END RSA PRIVATE KEY-----";
 */
const char AWS_ClientPrivateKey[] = "Paste client private key here.";
