package service

import (
	"crypto/ed25519"
	"crypto/rand"
	"encoding/base64"
)

type Signer struct {
	privateKey ed25519.PrivateKey
	publicKey  ed25519.PublicKey
}

// New EllipticCurve instance
func New() *Signer {
	publicKey, privateKey, _ := ed25519.GenerateKey(rand.Reader)

	return &Signer{
		publicKey:  publicKey,
		privateKey: privateKey,
	}
}

// EncodePrivate private key
func (ec *Signer) EncodePrivate() (key string) {

	return base64.StdEncoding.EncodeToString(ec.privateKey)
}
func (ec *Signer) EncodePublic() (key string) {

	return base64.StdEncoding.EncodeToString(ec.publicKey)
}

// EncodePublic public key
func (ec *Signer) Sign(hash []byte) string {
	return base64.StdEncoding.EncodeToString(ed25519.Sign(ec.privateKey, hash))
}
func (ec *Signer) Verify(hash []byte, signature string) bool {
	bytemessage, _ := base64.StdEncoding.DecodeString(signature)

	return ed25519.Verify(ec.publicKey, bytemessage, hash)
}
