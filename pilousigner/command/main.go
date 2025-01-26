package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
	"pilousigner/internal/configuration"
	"pilousigner/internal/model"
	"pilousigner/internal/service"
	"strings"

	"github.com/GoKillers/libsodium-go/sodium"
)

func main() {
	model.GenerateRandomString(5)
	var signer = service.New()
	var signedDictionary *model.SignedDictionary = model.CreateSignedDictionnary(signer)
	sodium.Init()
	err := filepath.Walk("/home/pilou/securephp/pilouloader",
		func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}
			if strings.HasSuffix(path, ".phpt") {
				fmt.Println(path, info.Size())
				var file, _ = os.Open(path)
				signedDictionary.AddFile(path, file)
				file.Close()
			}
			return nil
		})
	if err != nil {
		log.Println(err)
	}
	var signedConfiguration configuration.SignedConfiguration
	signedConfiguration.InitFromSignedDictionary(signedDictionary)
	signedConfiguration.WriteToJSONFile("test.json")
}
