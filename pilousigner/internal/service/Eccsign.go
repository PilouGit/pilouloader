package service

import (
	"crypto/ed25519"
	"crypto/rand"
	"encoding/base64"
	"log"

	"github.com/GoKillers/libsodium-go/cryptosign"
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
	log.Printf("Hash value in sign %s", base64.StdEncoding.EncodeToString(hash))

	signature, _ := cryptosign.CryptoSignDetached(hash, ec.privateKey)

	/*	log.Printf("Generating signature %s", base64.StdEncoding.EncodeToString(signature))
		if !ed25519.Verify(ec.publicKey, hash, signature) {
			log.Fatalf("Error in verify signature %s", base64.StdEncoding.EncodeToString(hash))
		}*/
	base64Signature := base64.StdEncoding.EncodeToString(signature)
	/*bytemessage, _ := base64.StdEncoding.DecodeString(base64Signature)

	if !ed25519.Verify(ec.publicKey, hash, bytemessage) {
		log.Fatalf("Error in verify signature %s", base64.StdEncoding.EncodeToString(hash))
	}*/
	return base64Signature
}
func (ec *Signer) Verify(hash []byte, signature string) bool {
	log.Printf("Verify signature value in verify %s", signature)
	log.Printf("Hash value in signing %s", base64.StdEncoding.EncodeToString(hash))
	bytemessage, _ := base64.StdEncoding.DecodeString(signature)

	return ed25519.Verify(ec.publicKey, hash, bytemessage)
}

func (ec *Signer) VerifyLibSodum(hash []byte, signature string) bool {
	log.Printf("Verify signature value in verify %s", signature)
	log.Printf("Hash value in verify %s", base64.StdEncoding.EncodeToString(hash))
	bytemessage, _ := base64.StdEncoding.DecodeString(signature)
	return cryptosign.CryptoSignVerifyDetached(bytemessage, hash, ec.publicKey) == 0
}
