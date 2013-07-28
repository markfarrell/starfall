function Main() {
    System.Log("Appearance", System.Xml2Json(new XmlDocument(System.Data("Entity/appearance.xml"))));
}