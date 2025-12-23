#!/bin/bash
# Teste simples do webhook GHD

GHD_URL="http://localhost:9000/hook"
SECRET="supersecret"

# JSON de exemplo
read -r -d '' PAYLOAD <<'EOF'
{
  "action": "issues",
  "repository": {"full_name": "49276d2072616d736f6d77617265/test_repo"},
  "ref": "refs/heads/main",
  "issue": {"number": 4"},
  "exec_dir":"/path/to/your/repo"
}
EOF

# Calcula HMAC
HMAC=$(printf '%s' "$PAYLOAD" | openssl dgst -sha256 -hmac "$SECRET" | sed 's/^.* //')

echo "Sending webhook with HMAC: $HMAC"

curl -i -X POST "$GHD_URL" \
  -H "Content-Type: application/json" \
  -H "X-Hub-Signature-256: sha256=$HMAC" \
  --data-binary "$PAYLOAD"
