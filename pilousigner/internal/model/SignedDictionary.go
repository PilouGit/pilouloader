package model

import (
	"crypto/rand"
	"crypto/sha256"
	"encoding/base64"
	"fmt"
	"io"
	"log"
	"pilousigner/internal/service"
)

type SignedFile struct {
	FileName string
	Hash     string
}

type SignedDictionary struct {
	data   []SignedFile
	key    string
	signer *service.Signer
}

func CreateSignedDictionnary(signer *service.Signer) *SignedDictionary {
	signedDictionnary := new(SignedDictionary)
	signedDictionnary.signer = signer
	signedDictionnary.key = signer.EncodePublic()
	return signedDictionnary

}
func (signedDictionary *SignedDictionary) AddFile(fileName string, f io.Reader) {
	h := sha256.New()
	if _, err := io.Copy(h, f); err != nil {
		log.Fatal(err)
	}

	hashValue := h.Sum(nil)
	signature := signedDictionary.signer.Sign(hashValue)
	/*if !signedDictionary.signer.Verify(hashValue, signature) {
		log.Fatalf("Error in verify signature %s", fileName)
	}*/
	if !signedDictionary.signer.VerifyLibSodum(hashValue, signature) {
		log.Fatalf("Error in verify signature in sodium %s", fileName)
	}
	var signedFile SignedFile
	signedFile.FileName = fileName
	signedFile.Hash = signature
	signedDictionary.data = append(signedDictionary.data, signedFile)

}
func (signedDictionary *SignedDictionary) GetPublicKey() string {
	return signedDictionary.key
}
func (signedDictionary *SignedDictionary) Size() int {
	return len(signedDictionary.data)
}
func (signedDictionary *SignedDictionary) GetSignedFile(i int) SignedFile {
	return signedDictionary.data[i]
}
func GenerateRandomString(length int) (string, error) {
	// Créer un tableau de bytes de la longueur souhaitée
	bytes := make([]byte, length)

	// Remplir le tableau avec des données aléatoires
	_, err := rand.Read(bytes)
	if err != nil {
		return "", fmt.Errorf("erreur lors de la génération aléatoire : %v", err)
	}

	// Convertir les bytes en chaîne hexadécimale
	return base64.StdEncoding.EncodeToString(bytes), nil
}
