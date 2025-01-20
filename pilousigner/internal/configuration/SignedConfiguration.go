package configuration

import (
	"encoding/json"
	"os"
	"pilousigner/internal/model"
)

type SignedFileConfiguration struct {
	FileName  string `json:"file"`
	Signature string `json:"signature"`
}
type SignedConfiguration struct {
	PublicKey               string                    `json:"publicKey"  `
	SignedFileConfiguration []SignedFileConfiguration `json:"signatures"`
}

func (signedConfiguration *SignedConfiguration) InitFromSignedDictionary(signedDictionary *model.SignedDictionary) {
	signedConfiguration.PublicKey = signedDictionary.GetPublicKey()
	for i := 0; i < signedDictionary.Size(); i++ {
		signedFile := signedDictionary.GetSignedFile(i)
		signedConfiguration.SignedFileConfiguration = append(signedConfiguration.SignedFileConfiguration, SignedFileConfiguration{FileName: signedFile.FileName,
			Signature: signedFile.Hash})
	}

}
func (signedConfiguration *SignedConfiguration) WriteToJSONFile(filename string) error {

	rankingsJson, _ := json.Marshal(signedConfiguration)
	err := os.WriteFile(filename, rankingsJson, 0644)

	return err
}
